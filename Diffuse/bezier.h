#ifndef bezier_h
#define bezier_h

#include "curve.h"

class bezier : public curve {
public:
    bezier(const list<point<float>*>& cpts, const unsigned int f = 50.0) {
        c_points = vector<point<float>*>(cpts.begin(), cpts.end());
        degree = (unsigned int)c_points.size() - 1;
        fidelity = f;
        set_uniform_param();
    }
    
    curvetype get_type() const { return curvetype::bezier; }
    
    void draw(const bool drawPoints) const {
        if (c_points.size() < 2)
            return;
        
        vector<point<float>> curve;
        for (unsigned int t = 0; t <= fidelity; t++)
            curve.push_back(decasteljau((float)t/(float)fidelity));
        
        glBegin(GL_QUAD_STRIP); {
            auto i2 = curve.begin(), i1 = i2++;
            while (i2 != curve.end()) {
                auto left = i1->leftside(*i2);
                auto right = i1->rightside(*i2);
                
                glColor3ub(i1->lColor[0], i1->lColor[1], i1->lColor[2]);
                glVertex2f(left.x, left.y);
                glColor3ub(i1->rColor[0], i1->rColor[1], i1->rColor[2]);
                glVertex2f(right.x, right.y);
                
                i1++;
                i2++;
            }
            auto right = i1->leftside(*(i1 - 1));
            auto left = i1->rightside(*(i1 - 1));
            
            glColor3ub(i1->lColor[0], i1->lColor[1], i1->lColor[2]);
            glVertex2f(left.x, left.y);
            glColor3ub(i1->rColor[0], i1->rColor[1], i1->rColor[2]);
            glVertex2f(right.x, right.y);
            
        } glEnd();
        
        if (drawPoints)
            for (auto &p : c_points)
                p->draw(0);
        
    }
    
    list<point<float>*>* elevate_degree() {
        auto *newpts = new list<point<float>*>();
        
        newpts->push_back(c_points.front());
        for (int i = 1; i < c_points.size(); i++)
            newpts->push_back(new point<float>(*c_points[i] * (1 - ((float)i / (float)(c_points.size()))) + *c_points[i - 1] * ((float)i / (float)(c_points.size()))));
        newpts->push_back(c_points.back());
        
        c_points = vector<point<float>*>(newpts->begin(), newpts->end());
        degree = (unsigned int)c_points.size() - 1;
        return newpts;
    }
    
private:
    point<float> decasteljau(const unsigned int d, const unsigned int begin, const float t, map<pair<unsigned int,unsigned int>,point<float>>& hash) const {
        if (d == 0)
            return *(c_points[begin]);
        
        auto p = pair<unsigned int, unsigned int>(d, begin);
        auto index = hash.find(p);
        
        if (index != hash.end())
            return index->second;
        
        return hash[p] = (decasteljau(d - 1, begin, t, hash) * (1.0 - t)) + (decasteljau(d - 1, begin + 1, t, hash) * t);
    }
    
    point<float> decasteljau(const float t) const {
        auto hash = map<pair<unsigned int, unsigned int>, point<float>>();
        auto p = decasteljau((unsigned int)c_points.size()-1, 0, t, hash);
        return p;
    }
};

#endif /* bezier_h */
