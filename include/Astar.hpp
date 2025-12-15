#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine/olcPixelGameEngine.h"

#include <iostream>
#include <cstdio>
#include <queue>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>

class AstarLittleGame : public olc::PixelGameEngine
{
private:
    struct Nodes
    {
        bool isvisited = false;
        bool isobstacle = false;
        float globalgoal;
        float localgoal;
        int x;
        int y;
        std::vector<Nodes *> vecneighbors;
        Nodes *parent;
    };

    Nodes *nodes = nullptr;
    const int map_width = 80;
    const int map_height = 60;

    Nodes *node_start = nullptr;
    Nodes *node_end = nullptr;


protected:

    virtual bool OnUserCreate()
    {   
        nodes = new Nodes[map_width * map_height];
        for (int x =  0; x < map_width; x++)
        {
            for (int y = 0; y < map_height; y++)
            {
               nodes[y * map_width + x].x = x;
               nodes[y * map_width + x].y = y;
               nodes[y * map_width + x].isobstacle = false;
               nodes[y * map_width + x].isvisited = false;
               nodes[y * map_width + x].parent = nullptr;
            }
        }

        for(int x = 0; x < map_width; x++)
        {
            for(int y = 0; y < map_height; y++)
            {
                if(y > 0)
                {
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y - 1) * map_width + (x + 0)]);
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y - 1) * map_width + (x - 1)]);
                }
                if(y < map_height - 1)
                {
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y + 1) * map_width + (x + 0)]);
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y + 1) * map_width + (x + 1)]);
                }
                if(x > 0)
                {
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y + 0) * map_width + (x - 1)]);
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y - 1) * map_width + (x - 1)]);
                }
                if(x < map_width - 1)
                {
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y + 0) * map_width + (x + 1)]);
                    nodes[y * map_width + x].vecneighbors.push_back(&nodes[(y + 1) * map_width + (x + 1)]);
                }
                
            }
        }
        node_start = &nodes[0];
        node_end = &nodes[map_width * map_height - 1];
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        int node_size = 6;
        int node_between = 6 * std::pow(0.618, 2);

        Clear(olc::GREY);

        int m_mouse_pos_x = GetMouseX();
        int m_mouse_pos_y = GetMouseY();
        int selectedNodeX = m_mouse_pos_x / node_size;
        int selectedNodeY = m_mouse_pos_y / node_size;

        if(GetMouse(0).bHeld)
        {
            if(selectedNodeX >= 0 && selectedNodeX < map_width)
            {
                if(selectedNodeY >= 0 && selectedNodeY < map_height)
                {   
                    if(GetKey(olc::Key::SHIFT).bHeld)
                    {
                        node_start = &nodes[selectedNodeY * map_width + selectedNodeX];
                    }
                    else if(GetKey(olc::Key::CTRL).bHeld)
                    {
                        node_end = &nodes[selectedNodeY * map_width + selectedNodeX];
                    }
                    else
                    {
                        nodes[selectedNodeY * map_width + selectedNodeX].isobstacle = 
                        !nodes[selectedNodeY * map_width + selectedNodeX].isobstacle;
                    }
                    
                    SolveAstar();
                    
                }        
            }
        }


        for(int x = 0; x < map_width; x++)
        {
            for(int y = 0; y < map_height; y++)
            {
                FillRect(x * node_size + node_between, y * node_size + node_between, 
                         node_size - node_between, node_size - node_between, 
                         olc::WHITE);
            }
        }

        for(int x = 0; x < map_width; x++)
        {
            for(int y = 0; y < map_height; y++)
            {
                if(&nodes[y * map_width + x] == node_start)
                {
                    FillRect(x * node_size + node_between, y * node_size + node_between, 
                             node_size - node_between, node_size - node_between, 
                             olc::GREEN);
                }
                else if(&nodes[y * map_width + x] == node_end)
                {
                    FillRect(x * node_size + node_between, y * node_size + node_between, 
                             node_size - node_between, node_size - node_between, 
                             olc::RED);
                }
                else if(nodes[y * map_width + x].isobstacle)
                {
                    FillRect(x * node_size + node_between, y * node_size + node_between, 
                             node_size - node_between, node_size - node_between, 
                             olc::DARK_GREY);
                }
            }
        }

        if(node_end != nullptr)
        {
            Nodes* p = node_end;
            while(p->parent != nullptr)
            {
                FillRect(p->x * node_size + node_between, p->y * node_size + node_between, 
                         node_size - node_between, node_size - node_between, 
                         olc::YELLOW);
                FillRect(node_end->x * node_size + node_between, node_end->y * node_size + node_between, 
                         node_size - node_between, node_size - node_between, 
                         olc::RED);
                p = p->parent;

            }
        }
    
        return true;
    }

    bool SolveAstar()
    {  
        for (int x =  0; x < map_width; x++)
        {
            for (int y = 0; y < map_height; y++)
            {
               nodes[y * map_width + x].isvisited = false;
               nodes[y * map_width + x].globalgoal = INFINITY;
               nodes[y * map_width + x].localgoal = INFINITY;
               nodes[y * map_width + x].parent = nullptr;
            }
        }

        auto distance = [](Nodes* a, Nodes*b)
        {
            return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
        };

        auto heuristic = [distance](Nodes* a, Nodes*b)
        {
            return distance(a, b);
        };

        Nodes* node_current = node_start;
        node_start->localgoal = 0.0f;
        node_start->globalgoal = heuristic(node_start, node_end);

        auto compare = [](Nodes* a, Nodes* b)
        {
            return a->globalgoal > b->globalgoal;
        };

        std::priority_queue<Nodes*, std::vector<Nodes*>, decltype(compare)> list_not_tested_nodes(compare);

        list_not_tested_nodes.push(node_start);
        
        while(!list_not_tested_nodes.empty() && node_current != node_end)
        {
            


            node_current = list_not_tested_nodes.top();
            list_not_tested_nodes.pop();

            if(node_current->isvisited)
            {
                continue;
            }

            node_current->isvisited = true;

            if(node_current == node_end)
            {
                break;
            }

            for(auto neighbor : node_current->vecneighbors)
            {

                if(!neighbor->isvisited && !neighbor->isobstacle)
                {
                    float fPossiblyLowerGoal = node_current->localgoal + distance(node_current, neighbor);

                    if(fPossiblyLowerGoal < neighbor->localgoal)
                    {
                        neighbor->parent = node_current;
                        neighbor->localgoal = fPossiblyLowerGoal;
                        neighbor->globalgoal = neighbor->localgoal + heuristic(neighbor, node_end);
                        list_not_tested_nodes.push(neighbor);
                    }
                }
            }
        }

        return true;
    }
};

