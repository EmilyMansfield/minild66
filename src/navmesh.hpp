#ifndef NAVMESH_HPP
#define NAVMESH_HPP

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <set>
#include <JsonBox.h>
#include <algorithm>
#include <iostream>

#include "graph.hpp"
#include "vecmath.hpp"

class ConvexPolygon
{
public:
    // Points defined in an anticlockwise direction
    // First point must be ac of the positive horizontal
    std::vector<sf::Vector2f> points;

    // Make an drawable sf::ConvexShape out of this to aid in
    // visualisation and debugging
    sf::ConvexShape getShape(float height)
    {
        sf::ConvexShape shape;
        shape.setPointCount(points.size());
        for(int i = 0; i < points.size(); ++i)
        {
            shape.setPoint(i,
                sf::Vector2f(points[i].x, height-points[i].y));
        }
        return shape;
    }

    // Helper function to add points
    void add(float x, float y)
    {
        points.push_back(sf::Vector2f(x, y));
    }

    // Loop through the points in anticlockwise order and draw
    // a vector between consecutive points. If the points is
    // left of every vector, it's in the polygon
    bool contains(const sf::Vector2f& point) const
    {
        for(int i = 0; i < points.size(); ++i)
        {
            auto p1 = points[i];
            auto p2 = points[(i+1)%points.size()];
            // To check the side of the line AB, take AB x AX
            // and look at the sign. If +ve, by the right hand rule
            // the point X is to the left of AB
            auto v = p2 - p1;
            auto u = point - p1;
            if(v.x*u.y - v.y*u.x < 0) return false;
        }

        return true;
    }

    sf::Vector2f centroid() const
    {
        sf::Vector2f cent(0.0f, 0.0f);
        for(int i = 0; i < points.size(); ++i)
        {
            // Assume that polygon is sensibly sized and has
            // few vertices, so that totalling the positions
            // does not introduce precision errors
            cent += points[i];
        }
        return cent / (float)points.size();
    }

    ConvexPolygon intersect(const ConvexPolygon& b) const
    {
        // Use a variant of the Sutherland-Hodgman algorithm
        // to calculate the intersection between two
        // convex polygons (which is again a convex polygon)
        ConvexPolygon clipped;
        for(int i = 0; i < b.points.size(); ++i)
        {
            auto p0 = b.points[i];
            // If A contains p0, p0 lies in the intersection
            if(this->contains(p0)) clipped.points.push_back(p0);
            // The line joining p_i and p_(i+1) will intersect
            // with A iff p_(i+1) lies in A and p_i does not.
            // If there is no intersection then the point can be ignored and
            // does not affect the clipped polygon.
            auto p1 = b.points[(i+1)%b.points.size()];
            if(this->contains(p0) && this->contains(p1)) continue;
            // Find the edge of node which intersects with the
            // edge of poly
            for(int j = 0; j < this->points.size(); ++j)
            {
                auto q0 = this->points[j];
                auto q1 = this->points[(j+1)%this->points.size()];
                sf::Vector2f intersection;
                if(vecmath::intersect(p0, p1, q0, q1, &intersection))
                {
                    // Add the intersection as a point
                    clipped.points.push_back(intersection);
                }
            }
        }
        // Add any A points which lie in B
        for(int i = 0; i < this->points.size(); ++i)
        {
            auto p0 = this->points[i];
            if(b.contains(p0)) clipped.points.push_back(p0);
        }
        // Calculate the centroid
        auto centroid = clipped.centroid();
        // Sort the points in anticlockwise order
        std::sort(clipped.points.begin(), clipped.points.end(),
            [&centroid](sf::Vector2f& a, sf::Vector2f& b) {
                return (a-centroid).x*(b-centroid).y - (a-centroid).y*(b-centroid).x < 0;
            });
        return clipped;
    }

    std::vector<ConvexPolygon> subtract(const ConvexPolygon& clippedPoly) const
    {
        // Find the centroid of clipped poly
        const sf::Vector2f centroid = clippedPoly.centroid();
        // Fire rays from the centroid through each vertex of the
        // clipped poly, and calculate the intersections between
        // the rays and the node
        std::vector<sf::Vector2f> intersections;
        for(auto p : clippedPoly.points)
        {
            auto& p0 = centroid;
            auto& p1 = p;
            for(int j = 0; j < this->points.size(); ++j)
            {
                auto& q0 = this->points[j];
                auto& q1 = this->points[(j+1)%this->points.size()];
                sf::Vector2f intersection;
                if(vecmath::intersect(p0, p1, q0, q1, &intersection, true))
                {
                    intersections.push_back(intersection);
                }
            }
        }
        std::cout << "Intersections = " << intersections.size() << std::endl;
        // The points in intersections and the points in this->points
        // form the points of the convex polygons which make up the
        // subtracted region. They must be taken in clockwise order,
        // forming closed loops. This is much easier understood when
        // drawn, so if you're reading this, go draw a diagram.
        // First make a combined vector containing the intersections
        // and the node points, orientated anti-clockwise
        std::set<sf::Vector2f, vecmath::vec2_compare<float>> intSet;
        std::vector<sf::Vector2f> totalPoints = this->points;
        auto lessThan = [&centroid](const sf::Vector2f& a, const sf::Vector2f& b) {
                return (a-centroid).x*(b-centroid).y - (a-centroid).y*(b-centroid).x > 0;
            };
        std::cout << std::string(50, '-') << std::endl;
        for(int j = 0; j < intersections.size(); ++j)
        {
            auto a = intersections[j];
            for(int k = 0; k < totalPoints.size(); ++k)
            {
                auto b0 = totalPoints[k];
                auto b1 = totalPoints[(k+1)%totalPoints.size()];
                // If a lies between b0 and b1, insert it after b0.
                // Since points are oriented ac, a should be to the
                // left of b0 and the right of b1
                if(lessThan(b0, a) && lessThan(a, b1))
                {
                    totalPoints.insert(totalPoints.begin()+k+1, a);
                    // std::cout << "Inserted " << "(" << a.x << ", " << a.y << ")"
                    //     << " between " << "(" << b0.x << ", " << b0.y << ")"
                    //     << " and " << "(" << b1.x << ", " << b1.y << ")\n";
                    break;
                }
            }
            intSet.insert(a);
        }
        // for(auto p : totalPoints)
        // {
        //     std::cout << "(" << p.x << ", " << p.y << ")\n";
        // }
        std::vector<ConvexPolygon> regions;
        ConvexPolygon region;
        std::set<sf::Vector2f, vecmath::vec2_compare<float>> addedPoints;
        int intIndex = 0;
        int n = 0;
        std::cout << "Total points = " << totalPoints.size() << std::endl;
        for(int j = 0; j < totalPoints.size(); j = (j+1) % totalPoints.size())
        {
            auto p = totalPoints[j];
            // Add the jth point if it hasn't already been added
            if(addedPoints.count(p) == 0)
            {
                addedPoints.insert(p);
                region.points.push_back(p);
            }
            // If it has, then the region is complete
            else
            {
                regions.push_back(region);
                // If every point has been visited, then we are
                // done
                if(addedPoints.size() >= totalPoints.size())
                {
                    break;
                }
                // Otherwise we repeat for a new region, starting
                // from the point we remembered earlier
                region = ConvexPolygon();
                // addedPoints.clear();
                // We have to add the point now then continue
                // so that it is not checked as an intersection
                // and closed into the wrong region
                // addedPoints.insert(totalPoints[intIndex]);
                region.points.push_back(totalPoints[intIndex]);
                j = intIndex;
                continue;
            }
            // If this point is an intersection point, we need to
            // close the region up
            if(intSet.count(p) == 1)
            {
                // Remember this position, it will be the start
                // position for the next region
                intIndex = j;
                // Move along the intersection line back to the
                // centroid, and add the poly point. To do this,
                // find the poly point with the same angle as
                // this point. it might be the same point as the
                // intersection, if so don't add it
                int k;
                for(k = 0; k < clippedPoly.points.size(); ++k)
                {
                    if(std::abs(
                        vecmath::angle(clippedPoly.points[k]-centroid) -
                        vecmath::angle(p-centroid)) < 10e-5)
                    {
                        break;
                    }
                }
                if(vecmath::norm(clippedPoly.points[k]-p) > 10e-5)
                {
                    region.points.push_back(clippedPoly.points[k]);
                }
                // Now move clockwise around the poly and add
                // the point before
                int prevK = (k > 0 ? k-1 : clippedPoly.points.size()-1);
                region.points.push_back(clippedPoly.points[prevK]);
                // Move along this intersection line back to
                // the edge of the node, adding that point if
                // needed (as before)
                if(vecmath::norm(clippedPoly.points[prevK]-intersections[prevK]) > 10e-5)
                {
                    // This might already have been added as the first
                    // point in the region
                    if(addedPoints.count(intersections[prevK]) == 0)
                    {
                        region.points.push_back(intersections[prevK]);
                    }
                }
                // This point lies on the edge, so it needs
                // to be recorded
                addedPoints.insert(intersections[prevK]);
                // Decrement j until we find the point
                // corresponding to intersections[prevK]
                while(vecmath::norm(totalPoints[j]-intersections[prevK]) > 10e-5)
                {
                    j = (j > 0 ? j-1 : totalPoints.size()-1);
                }
            }
        }
        // Regions now contains the convex regions that our
        // original node has been split up into
        return regions;
    }
};

// A Navmesh is a graph of convex polygons
class Navmesh
{
public:
    Graph<ConvexPolygon> polys;

    Navmesh() {}
    explicit Navmesh(const JsonBox::Value& v)
    {
        load(v);
    }

    void load(const JsonBox::Value& v)
    {
        JsonBox::Object o = v.getObject();
        if(o.find("nodes") != o.end())
        {
            JsonBox::Array nodes = o["nodes"].getArray();
            for(auto node : nodes)
            {
                ConvexPolygon poly;
                JsonBox::Array nodePoints = node.getArray();
                for(auto point : nodePoints)
                {
                    JsonBox::Array components = point.getArray();
                    // TODO: Array bounds checking
                    poly.points.push_back(sf::Vector2f(
                                components[0].tryGetFloat(0.0f),
                                components[1].tryGetFloat(0.0f)));
                }
                polys.add_node(poly);
            }
        }
        if(o.find("edges") != o.end())
        {
            JsonBox::Array edges = o["edges"].getArray();
            for(auto edge : edges)
            {
                // TODO: Array bounds checking
                JsonBox::Array components = edge.getArray();
                polys.add_edge(
                        components[0].tryGetInteger(0),
                        components[1].tryGetInteger(0),
                        false);
            }
        }
    }

    // Subtract a convex polygon from the navmesh
    void subtract(const ConvexPolygon& poly)
    {
        std::vector<int> nodesIntersectingPoly;
        // Poly may lie in more than one node, so first find
        // every node that poly lies in
        for(auto p : poly.points)
        {
            for(int i = 0; i < polys.nodes.size(); ++i)
            {
                if(polys.nodes[i].data.contains(p))
                {
                    nodesIntersectingPoly.push_back(i);
                    break;
                }
            }
        }
        // Now for every node which contained at least part of poly,
        // replace poly with the intersection of poly and the node.
        // Note that the intersection of two convex polygons is again a
        // convex polygon, so this is valid.
        for(auto i : nodesIntersectingPoly)
        {
            const ConvexPolygon& node = polys.nodes[i].data;
            ConvexPolygon clippedPoly;
            // Save some time by not performing polygon clipping if
            // poly lies entirely within node
            bool allIn = true;
            for(int j = 0; j < poly.points.size(); ++j)
            {
                if(!node.contains(poly.points[j]))
                {
                    allIn = false;
                    break;
                }
            }
            if(!allIn)
            {
                // Polygon intersects so need its intersection
                clippedPoly = node.intersect(poly);
            }
            else
            {
                // Contained within so no intersection
                clippedPoly = poly;
            }
            auto subnodes = node.subtract(clippedPoly);
        }
    }
};

#endif /* NAVMESH_HPP */
