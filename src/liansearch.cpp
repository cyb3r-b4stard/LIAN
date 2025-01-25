#include "liansearch.h"

/*
 * // Use for more accurate time calculation
 * #ifdef __linux__
 *     #include <sys/time.h>
 * #else
 *     #include <windows.h>
 * #endif
 *
*/

LianSearch::~LianSearch() {}


LianSearch::LianSearch(float angleLimit_, int distance_, float weight_,
    unsigned int stepLimit_, float curvatureHeuristicWeight_, bool postSmoother_,
    float decreaseDistanceFactor_, int distanceMin_,
    double pivotRadius_, int numOfParentsToIncreaseRadius_) {
    this->angleLimit = angleLimit_;
    this->distance = distance_;
    this->weight = weight_;
    this->stepLimit = stepLimit_;
    this->curvatureHeuristicWeight = curvatureHeuristicWeight_;
    this->postSmoother = postSmoother_;
    this->decreaseDistanceFactor = decreaseDistanceFactor_;
    this->distanceMin = distanceMin_;
    this->pivotRadius = pivotRadius_;
    this->numOfParentsToIncreaseRadius = numOfParentsToIncreaseRadius_;
    // closeSize = 0;
    srand(time(NULL));
}

/*
    TODO: Compute theoretical upper-bound for number of pushed circle nodes.
    Use for determining in advance suitable size of vector, this will
    decrease potential drawbacks from having to constantly resize vector.

*/
void LianSearch::calculateCircle(int radius) { //here radius - radius of the circle in cells
    circleNodes.clear();
    circleNodes.resize(listOfDistancesSize);
    
    for (int k = 0; k < listOfDistancesSize; ++k) {
        radius = listOfDistances[k];
        circleNodes[k].clear();
        std::vector<circleNode> circle_nodes(0);
        int x = 0;
        int y = radius;
        int delta = 2 - 2 * radius;
        int error = 0;
        while (y >= 0) {
            if (x > radius) x = radius;
            else if (x < -radius) x = -radius;
            if (y > radius) y = radius;
            else if (y < -radius) y = -radius;
            double dist = getCost(0, 0, x, y);
            circle_nodes.emplace_back(circleNode(x, y, dist));
            circle_nodes.emplace_back(circleNode(x, -y, dist));
            circle_nodes.emplace_back(circleNode(-x, y, dist));
            circle_nodes.emplace_back(circleNode(-x, -y, dist));

            error = 2 * (delta + y) - 1;
            if (delta < 0 && error <= 0) {
                delta += 2 * ++x + 1;
                continue;
            }

            error = 2 * (delta - x) - 1;
            if (delta > 0 && error > 0) {
                delta += 1 - 2 * --y;
                continue;
            }
            delta += 2 * (++x - y--);
        }

        for (int i = 0; i < circle_nodes.size(); i += 4)
            circleNodes[k].emplace_back(circle_nodes[i]);
        for (int i = circle_nodes.size() - 7; i > 0; i -= 4)
            circleNodes[k].emplace_back(circle_nodes[i]);
        for (int i = 7; i < circle_nodes.size(); i += 4)
            circleNodes[k].emplace_back(circle_nodes[i]);
        for (int i = circle_nodes.size() - 6; i > 0; i -= 4)
            circleNodes[k].emplace_back(circle_nodes[i]);
        circleNodes[k].pop_back();
        for (size_t i = 0; i < circleNodes[k].size(); ++i) {
            double angle = acos((circleNodes[k][0].i * circleNodes[k][i].i + circleNodes[k][0].j * circleNodes[k][i].j)
                / (sqrt(pow(circleNodes[k][0].i, 2) + pow(circleNodes[k][0].j, 2)) 
                    * sqrt(pow(circleNodes[k][i].i, 2) + pow(circleNodes[k][i].j, 2)))); 
            if (i < circleNodes[k].size() / 2)
                circleNodes[k][i].heading = angle * 180 / CN_PI_CONSTANT;
            else
                circleNodes[k][i].heading = 360 - angle * 180 / CN_PI_CONSTANT;
            //std::cout << circleNodes[k][i].heading  << std::endl;
        }
    }
}

// TODO: Statically resize vector instead of emplace_back's
void LianSearch::calculatePivotCircle() {
    pivotCircle.clear();
    int add_i, add_j, num(pivotRadius + 0.5 - CN_EPSILON);
    for (int i = -num; i <= +num; i++) {
        for (int j = -num; j <= +num; j++) {
            add_i = i != 0 ? 1 : 0;
            add_j = j != 0 ? 1 : 0;
            if ((pow(2 * abs(i) - add_i, 2) + pow(2 * abs(j) - add_j, 2)) < pow(2 * pivotRadius, 2))
                pivotCircle.emplace_back(i, j);
        }
    }
    if (pivotCircle.empty())
        pivotCircle.emplace_back(0, 0);
}

bool LianSearch::checkPivotCircle(const Map& map, const Node& center) {
    int i, j;
    for (int k = 0; k < pivotCircle.size(); k++) {
        i = center.i + pivotCircle[k].first;
        j = center.j + pivotCircle[k].second;
        if (!map.cellOnGrid(i, j) || map.cellIsObstacle(i, j))
            return false;
    }
    return true;
}

void LianSearch::calculateDistances() {
    int curDistance = distance;
    if (decreaseDistanceFactor > 1) {
        while (curDistance >= distanceMin) {
            listOfDistances.emplace_back(curDistance);
            curDistance = ceil(curDistance / decreaseDistanceFactor);
        }
    } else {
        listOfDistances.emplace_back(curDistance);
    }
    listOfDistancesSize = listOfDistances.size();
}

void LianSearch::calculateLineSegment(std::vector<Node>& line, const Node& start, const Node& goal) {
    int x1 = start.i;
    int y1 = start.j;
    int x2 = goal.i;
    int y2 = goal.j;

    int x, y;
    int dx, dy;
    int stepVal = 0;
    int rotate = 0;

    line.clear();

    if (x1 > x2 && y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);

        dx = x2 - x1;
        dy = y2 - y1;
    } else {
        dx = x2 - x1;
        dy = y2 - y1;

        if (dx >= 0 && dy >= 0) rotate = 2;
        else if (dy < 0) {
            dy = -dy;
            std::swap(y1, y2);

            rotate = 1;
        } else if (dx < 0) {
            dx = -dx;
            std::swap(x1, x2);

            rotate = 3;
        }
    }

    if (rotate == 1) {
        if (dx >= dy) {
            for (x = x1; x <= x2; ++x) {
                line.emplace_back(Node(x, y2));
                stepVal += dy;
                if (stepVal >= dx) {
                    --y2;
                    stepVal -= dx;
                }
            }
        } else {
            for (y = y1; y <= y2; ++y) {
                line.insert(line.begin(), Node(x2, y));
                stepVal += dx;
                if (stepVal >= dy) {
                    --x2;
                    stepVal -= dy;
                }
            }
        }
        return;
    } else if (rotate == 2) {
        if (dx >= dy) {
            for (x = x1; x <= x2; ++x) {
                line.emplace_back(Node(x, y1));
                stepVal += dy;
                if (stepVal >= dx) {
                    ++y1;
                    stepVal -= dx;
                }
            }
            return;
        } else {
            for (y = y1; y <= y2; ++y) {
                line.emplace_back(Node(x1, y));
                stepVal += dx;
                if (stepVal >= dy) {
                    ++x1;
                    stepVal -= dy;
                }
            }
            return;
        }
    } else if (rotate == 3) {
        if (dx >= dy) {
            for (x = x1; x <= x2; ++x) {
                line.insert(line.begin(), Node(x, y2));
                stepVal += dy;
                if (stepVal >= dx) {
                    --y2;
                    stepVal -= dx;
                }
            }
        } else {
            for (y = y1; y <= y2; ++y) {
                line.emplace_back(Node(x2, y));
                stepVal += dx;
                if (stepVal >= dy) {
                    --x2;
                    stepVal -= dy;
                }
            }
        }
        return;
    }

    if (dx >= dy) {
        for (x = x1; x <= x2; ++x) {
            line.insert(line.begin(), Node(x, y1));
            stepVal += dy;
            if (stepVal >= dx) {
                ++y1;
                stepVal -= dx;
            }
        }
    } else {
        for (y = y1; y <= y2; ++y) {
            line.insert(line.begin(), Node(x1, y));
            stepVal += dx;
            if (stepVal >= dy) {
                ++x1;
                stepVal -= dy;
            }
        }
    }
}

bool LianSearch::checkLineSegment(const Map& map, const Node& start, const Node& goal) {
    int x1 = start.i;
    int y1 = start.j;
    int x2 = goal.i;
    int y2 = goal.j;

    int x, y;
    int dx, dy;
    int stepVal = 0;
    int rotate = 0;

    if (x1 > x2 && y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);

        dx = x2 - x1;
        dy = y2 - y1;
    } else {
        dx = x2 - x1;
        dy = y2 - y1;

        if (dx >= 0 && dy >= 0) rotate = 2;
        else if (dy < 0) {
            dy = -dy;
            std::swap(y1, y2);
            rotate = 1;
        } else if (dx < 0) {
            dx = -dx;
            std::swap(x1, x2);
            rotate = 3;
        }
    }

    if (rotate == 1) {
        if (dx >= dy) {
            for (x = x1; x <= x2; ++x) {
                if (map.cellIsObstacle(x, y2)) return false;
                stepVal += dy;
                if (stepVal >= dx) {
                    --y2;
                    stepVal -= dx;
                }
            }
        } else {
            for (y = y1; y <= y2; ++y) {
                if (map.cellIsObstacle(x2, y)) return false;
                stepVal += dx;
                if (stepVal >= dy) {
                    --x2;
                    stepVal -= dy;
                }
            }
        }
        return true;
    } else if (rotate == 2) {
        if (dx >= dy) {
            y = y1;
            for (x = x1; x <= x2; ++x) {
                if (map.cellIsObstacle(x, y1)) return false;
                stepVal += dy;
                if (stepVal >= dx) {
                    ++y1;
                    stepVal -= dx;
                }
            }
            return true;
        } else {
            for (y = y1; y <= y2; ++y) {
                if (map.cellIsObstacle(x1, y)) return false;
                stepVal += dx;
                if (stepVal >= dy) {
                    ++x1;
                    stepVal -= dy;
                }
            }
            return true;
        }
    } else if (rotate == 3) {
        if (dx >= dy) {
            for (x = x1; x <= x2; ++x) {
                if (map.cellIsObstacle(x, y2)) return false;
                stepVal += dy;
                if (stepVal >= dx) {
                    --y2;
                    stepVal -= dx;
                }
            }
        } else {
            for (y = y1; y <= y2; ++y) {
                if (map.cellIsObstacle(x2, y)) return false;
                stepVal += dx;
                if (stepVal >= dy) {
                    --x2;
                    stepVal -= dy;
                }
            }
        }
        return true;
    }

    if (dx >= dy) {
        for (x = x1; x <= x2; ++x) {
            if (map.cellIsObstacle(x, y1)) return false;
            stepVal += dy;
            if (stepVal >= dx) {
                ++y1;
                stepVal -= dx;
            }
        }
    } else {
        for (y = y1; y <= y2; ++y) {
            if (map.cellIsObstacle(x1, y)) return false;
            stepVal += dx;
            if (stepVal >= dy) {
                ++x1;
                stepVal -= dy;
            }
        }
    }
    return true;
}

bool LianSearch::stopCriterion() {
    if (searchTree.isOpenEmpty()) {
        std::cout << "OPEN list is empty!" << std::endl;
        return true;
    }

    if (searchTree.getClosedSize() > stepLimit && stepLimit > 0) {
        std::cout << "Algorithm exceeded step limit!" << std::endl;
        return true;
    }
    return false;
}

double LianSearch::getCost(int a_i, int a_j, int b_i, int b_j) const {
    return std::hypot(a_i - b_i, a_j - b_j);
}


double LianSearch::calcAngle(const Node& dad, const Node& node, const Node& son) const {
    double angle_first  = atan2(node.j - dad.j, node.i - dad.i);
    double angle_second = atan2(son.j - node.j, son.i - node.i);

    if (angle_first < 0)
        angle_first += 2 * CN_PI_CONSTANT;

    if (angle_second < 0)
        angle_second += 2 * CN_PI_CONSTANT;

    return angle_second - angle_first;
}

SearchResult LianSearch::startSearch(Logger* logger, const Map& map) {
    calculateDistances();

    std::cout << "List of distances :";
    for (auto dist : listOfDistances) {
        std::cout << " " << dist;
    }
    std::cout << std::endl;

    searchTree.resizeOpen(map.getHeight());
    Node curNode(map.start_i, map.start_j, 0.0, 0.0, 0.0);
    curNode.radius = distance;
    curNode.f = weight * getCost(curNode.i, curNode.j, map.goal_i, map.goal_j);
    bool pathFound = false;
    searchTree.addToOpen(curNode);
    calculateCircle((int)curNode.radius);
    calculatePivotCircle();

    std::chrono::time_point<std::chrono::system_clock> begin, end;
    begin = std::chrono::system_clock::now();

    /*
     * #ifdef __linux__
     *     timeval begin, end;
     *     gettimeofday(&begin, NULL);
     * #else
     *     LARGE_INTEGER begin,end,freq;
     *     QueryPerformanceCounter(&begin);
     *     QueryPerformanceFrequency(&freq);
     * #endif
     */

    while (!stopCriterion()) { // main cycle of the search
        curNode = searchTree.getMin();
        
        searchTree.addToClosed(curNode, map.getWidth());

        if (logger->logLevel == CN_LOGLVL_ITER) logger->writeToLogIter(searchTree.getClosedSize(), curNode);

        if (curNode.i == map.goal_i && curNode.j == map.goal_j) { // if current point is goal point - end of the cycle
            pathFound = true;
            break;
        }

        if (!expand(curNode, map) && listOfDistancesSize > 1)
            while (curNode.radius > listOfDistances[listOfDistancesSize - 1])
                if (tryToDecreaseRadius(curNode, map.getWidth()))
                    if (expand(curNode, map))
                        break;

        if (logger->logLevel >= CN_LOGLVL_LOW) logger->writeToLogOpenClose(searchTree);
    }

    if (logger->logLevel == CN_LOGLVL_MED) logger->writeToLogOpenClose(searchTree);

    sResult.nodesCreated = searchTree.getOpenSize() + searchTree.getClosedSize();
    sResult.numberOfSteps = searchTree.getClosedSize();

    if (pathFound) {
        sResult.pathLength = curNode.g;

        makePrimaryPath(curNode);
        
        if (postSmoother) {
            hpPath = smoothPath(hpPath, map);
        }
        
        makeSecondaryPath();
        
        float max_angle = makeAngles();
        
        sResult.pathFound = true;
        sResult.hpPath = hpPath;
        sResult.lpPath = lpPath;
        sResult.angles = angles;
        sResult.maxAngle = max_angle;
        sResult.sections = hpPath.size() - 1;

        end = std::chrono::system_clock::now();
        sResult.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1000000000;
        /* // for more accurate time calculation
       #ifdef __linux__
           gettimeofday(&end, NULL);
           sresult.time = (end.tv_sec - begin.tv_sec) + static_cast<double>(end.tv_usec - begin.tv_usec) / 1000000;
       #else
           QueryPerformanceCounter(&end);
           sresult.time = static_cast<double long>(end.QuadPart-begin.QuadPart) / freq.QuadPart;
       #endif */

        return sResult;
    } else {
        sResult.pathFound = false;

        end = std::chrono::system_clock::now();
        sResult.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1000000000;

        /* for more accurate time calculation
       #ifdef __linux__
           gettimeofday(&end, NULL);
           sresult.time = (end.tv_sec - begin.tv_sec) + static_cast<double>(end.tv_usec - begin.tv_usec) / 1000000;
       #else
           QueryPerformanceCounter(&end);
           sresult.time = static_cast<double long>(end.QuadPart-begin.QuadPart) / freq.QuadPart;
       #endif */

        return sResult;
    }
}

int LianSearch::tryToIncreaseRadius(Node curNode) {
    bool change = false;
    int i, k = 0;

    while (k < numOfParentsToIncreaseRadius) {
        if (curNode.parent != NULL) {
            if (curNode.radius == curNode.parent->radius) {
                ++k;
                curNode = *curNode.parent;
                continue;
            }
        }
        break;
    }
    if (k == numOfParentsToIncreaseRadius) {
        for (i = listOfDistancesSize - 1; i >= 0; --i)
            if (curNode.radius == listOfDistances[i]) break;
        if (i > 0) change = true;
    }
    if (change) return listOfDistances[i - 1];
    else return curNode.radius;
}

void LianSearch::update(const Node curNode, Node newNode, bool& successors, const Map& map) {
    if (!checkLineSegment(map, *newNode.parent, newNode)) return;
    if (pivotRadius > 0 && (newNode.i != map.goal_i || newNode.j != map.goal_j) && !checkPivotCircle(map, newNode)) return;

    auto it = searchTree.closed.find(newNode.convolution(map.getWidth()));

    if (it != searchTree.closed.end()) {
        auto range = searchTree.closed.equal_range(it->first);
        for (auto it = range.first; it != range.second; ++it)
            if (it->second.parent == nullptr || (it->second.parent->i == curNode.i && it->second.parent->j == curNode.j))
                return;
    }

    if (listOfDistancesSize > 1) newNode.radius = tryToIncreaseRadius(newNode);
    searchTree.addToOpen(newNode);
    successors = true;
}

bool LianSearch::expand(const Node curNode, const Map& map) {
    int curDist;

    for (curDist = 0; curDist < listOfDistancesSize; ++curDist)
        if (listOfDistances[curDist] == curNode.radius)
            break;

    std::vector<circleNode> curCircleNodes = circleNodes[curDist];

    bool successorsAreFine = false;
    auto parent = &(searchTree.closed.find(curNode.convolution(map.getWidth()))->second);

    if (curNode.parent != nullptr) {
        int nodeStraightAhead = (int)round(curNode.angle * circleNodes[curDist].size() / 360) % circleNodes[curDist].size();

        double angle = fabs(curNode.angle - circleNodes[curDist][nodeStraightAhead].heading);
        
        if ((angle <= 180 && angle <= angleLimit) || (angle > 180 && 360 - angle <= angleLimit)) {
            int new_pos_i = curNode.i + curCircleNodes[nodeStraightAhead].i;
            int new_pos_j = curNode.j + curCircleNodes[nodeStraightAhead].j;
            
            if (map.cellOnGrid(new_pos_i, new_pos_j) && map.cellIsTraversable(new_pos_i, new_pos_j)) {
                Node newNode = Node(new_pos_i, new_pos_j);
                newNode.g = curNode.g + getCost(curNode.i, curNode.j, new_pos_i, new_pos_j);
                newNode.angle = circleNodes[curDist][nodeStraightAhead].heading;
                newNode.f = newNode.g + weight * getCost(new_pos_i, new_pos_j, map.goal_i, map.goal_j) +
                    curvatureHeuristicWeight * distance * fabs(curNode.angle - newNode.angle);
                newNode.radius = curNode.radius;
                newNode.parent = parent;

                update(curNode, newNode, successorsAreFine, map);
            }
        } // now we will expand neighbors that are closest to the node that lies straight ahead

        std::vector<int> candidates = std::vector<int>{ nodeStraightAhead, nodeStraightAhead };

        bool limit1 = true;
        bool limit2 = true;
        
        while (++candidates[0] != --candidates[1] && (limit1 || limit2)) { // until the whole circle is explored or we exceeded anglelimit somewhere
            if (candidates[0] >= curCircleNodes.size()) candidates[0] = 0;
            if (candidates[1] < 0) candidates[1] = curCircleNodes.size() - 1;

            for (auto cand : candidates) {
                double angle = fabs(curNode.angle - circleNodes[curDist][cand].heading);
                if ((angle <= 180 && angle <= angleLimit) || (angle > 180 && 360 - angle <= angleLimit)) {
                    int new_pos_i = curNode.i + curCircleNodes[cand].i;
                    int new_pos_j = curNode.j + curCircleNodes[cand].j;

                    if (!map.cellOnGrid(new_pos_i, new_pos_j)) continue;
                    if (map.cellIsObstacle(new_pos_i, new_pos_j)) continue;

                    Node newNode = Node(new_pos_i, new_pos_j);
                    newNode.g = curNode.g + getCost(curNode.i, curNode.j, new_pos_i, new_pos_j);
                    newNode.angle = circleNodes[curDist][cand].heading;
                    newNode.f = newNode.g + weight * getCost(new_pos_i, new_pos_j, map.goal_i, map.goal_j) +
                        curvatureHeuristicWeight * distance * fabs(curNode.angle - newNode.angle);
                    newNode.radius = curNode.radius;
                    newNode.parent = parent;

                    update(curNode, newNode, successorsAreFine, map);
                } else {
                    if (cand == candidates[0]) limit1 = false;
                    else limit2 = false;
                }
            }
        }
    } else { // when we do not have parent, we should explore all neighbors
        int anglePos(-1), newPos_i, newPos_j;

        for (auto node : curCircleNodes) {
            newPos_i = curNode.i + node.i;
            newPos_j = curNode.j + node.j;
            anglePos++;

            if (!map.cellOnGrid(newPos_i, newPos_j)) continue;
            if (map.cellIsObstacle(newPos_i, newPos_j)) continue;

            Node newNode = Node(newPos_i, newPos_j);
            newNode.g = curNode.g + getCost(curNode.i, curNode.j, newPos_i, newPos_j);
            newNode.f = newNode.g + weight * getCost(newPos_i, newPos_j, map.goal_i, map.goal_j);
            newNode.radius = curNode.radius;
            newNode.angle = circleNodes[curDist][anglePos].heading;
            newNode.parent = parent;

            update(curNode, newNode, successorsAreFine, map);
        }
    }

    // when we are near goal point, we should try to reach it
    if (getCost(curNode.i, curNode.j, map.goal_i, map.goal_j) <= curNode.radius) {
        double angle;
        if (curNode.parent == nullptr) {
            angle = 0;
        } else {
            angle = calcAngle(*curNode.parent, curNode, Node(map.goal_i, map.goal_j));
        }

        if (fabs(angle * 180 / CN_PI_CONSTANT) <= angleLimit) {
            Node newNode = Node(map.goal_i, map.goal_j,
                curNode.g + getCost(curNode.i, curNode.j, map.goal_i, map.goal_j), 0.0,
                curNode.radius, parent, curvatureHeuristicWeight * distance, 0.0);

            update(curNode, newNode, successorsAreFine, map);
        }
    }
    return successorsAreFine;
}


bool LianSearch::tryToDecreaseRadius(Node& curNode, int width) {
    int i;

    for (i = listOfDistancesSize - 1; i >= 0; --i)
        if (curNode.radius == listOfDistances[i]) break;
    
    if (i < listOfDistancesSize - 1) {
        curNode.radius = listOfDistances[i + 1];
        
        auto it = searchTree.closed.find(curNode.convolution(width));
        auto range = searchTree.closed.equal_range(it->first);
        
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second.parent && it->second.parent->i == curNode.parent->i
                && it->second.parent->j == curNode.parent->j) {
                it->second.radius = listOfDistances[i + 1];
                break;
            }
        }
        return true;
    }
    return false;
}

void LianSearch::makePrimaryPath(Node curNode) {
    hpPath.emplace_front(curNode);
    
    while (curNode.parent != nullptr) {
        curNode = *curNode.parent;
        hpPath.emplace_front(curNode);
    }
    hpPath.emplace_front(curNode);
}

bool LianSearch::checkAngle(const Node& dad, const Node& node, const Node& son) const {
    double angle = calcAngle(dad, node, son) * 180 / CN_PI_CONSTANT;
    
    if (fabs(angle) <= angleLimit) {
        return true;
    }
    
    return false;
}


std::list<Node> LianSearch::smoothPath(const std::list<Node>& path, const Map& map) {
    std::list<Node> new_path;
    sResult.pathLength = 0;
    auto it = path.begin();
    auto curr_it = path.begin();
    Node start_section = path.front();
    Node end_section = path.front();
    bool first = true;
    Node previous = *it++;
    while (end_section != path.back()) {
        for (it; it != path.end(); ++it) {
            auto next = ++it;
            --it;
            if (!first && !checkAngle(previous, start_section, *it)) continue;
            if ((next != path.end() && checkAngle(start_section, *it, *next) ||
                next == path.end()) && checkLineSegment(map, start_section, *it)) {
                end_section = *it;
                curr_it = it;
            }
        }
        sResult.pathLength += getCost(previous.i, previous.j, start_section.i, start_section.j);
        new_path.emplace_back(start_section);
        previous = start_section;
        first = false;
        start_section = end_section;
        it = ++curr_it;
    }
    sResult.pathLength += getCost(previous.i, previous.j, end_section.i, end_section.j);
    new_path.emplace_back(end_section);
    return new_path;
}

void LianSearch::makeSecondaryPath() {
    std::vector<Node> lineSegment;
    auto it = hpPath.begin();
    Node parent = *it++;
    while (it != hpPath.end()) {
        calculateLineSegment(lineSegment, parent, *it);
        std::reverse(std::begin(lineSegment), std::end(lineSegment));
        lpPath.insert(lpPath.begin(), ++lineSegment.begin(), lineSegment.end());
        parent = *it++;
    }
    lpPath.push_front(hpPath.back());
    std::reverse(std::begin(lpPath), std::end(lpPath));
}

double LianSearch::makeAngles() {
    angles.clear();
    double max_angle = 0;
    sResult.accumAngle = 0;
    auto pred = hpPath.begin();
    auto current = ++hpPath.begin();
    auto succ = ++(++hpPath.begin());

    while (succ != hpPath.end()) {
        double angle = calcAngle(*pred++, *current++, *succ++);
        angle = angle * 180 / CN_PI_CONSTANT;
        if (angle > max_angle) max_angle = angle;
        sResult.accumAngle += angle;
        angles.emplace_back(angle);
    }

    std::reverse(std::begin(angles), std::end(angles));

    return max_angle;
}
