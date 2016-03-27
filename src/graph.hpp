/*
The MIT License (MIT)

Copyright (c) 2013 Daniel Mansfield

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

template <class T>
class Node
{
    public:

    T data;

    // Make a new node and initialise it with the given data
    explicit Node(T data) : data(data) {}
};

template <class T>
class Edge
{
    public:

    // Array of nodes the edge joins together
    // nodes[0] is joined to nodes[1]
    Node<T>* nodes[2];

    // Weight associated with the edge
    double weight;

    // Join node a to node b with a directed edge
    // Default to not having a weight
    Edge(Node<T>* a, Node<T>* b, double weight=0.0)
    {
	    this->nodes[0] = a;
	    this->nodes[1] = b;
        this->weight = weight;
    }
};

template <class T>
class Graph
{
    public:

    // Vectors of nodes and edges
    // Change >> to > > to get this to run for non-C++11
    std::vector<Node<T>> nodes;
    std::vector<Edge<T>> edges;

    // Make an empty graph
    Graph() {}

    // Add a new vertex to the graph, without forming any edges
    void add_node(T data)
    {
	    // Create a new node
	    Node<T> node(data);

	    // Add it to the node list
	    this->nodes.push_back(node);
    }

    // Add an edge from a to b, or also from b to a if the edge is
    // undirected. Default to being directed
    void add_edge(Node<T>* a, Node<T>* b, bool directed=true, double weight=0.0)
    {
        // Add a new edge from a to b
	    Edge<T> edge1(a, b, weight);
	    this->edges.push_back(edge1);
        // Add an edge from b to a if necessary
	    if(!directed)
	    {
		    Edge<T> edge2(b, a, weight);
		    this->edges.push_back(edge2);
	    }
    }

    // Add an edge from a to b, using array indices instead of pointers
    void add_edge(unsigned int a, unsigned int b, bool directed=true, double weight=0.0)
    {
        // Convert the array indices into pointers
        Node<T>* aPtr = &this->nodes[a];
        Node<T>* bPtr = &this->nodes[b];

        // Call the usual function to add the edge
        add_edge(aPtr, bPtr, directed, weight);
    }

    // Add a new vertex to the graph, and add an edge from parent
    // to child. Default to being directed
    void add_node(T data, Node<T>* parent, bool directed=true, double weight=0.0)
    {
        // Create a new node
	    Node<T> node(data);

        // Add it to the node list
	    this->nodes.push_back(node);

	    // Create an edge between the new node and the parent
	    this->add_edge(parent, &(this->nodes.back()), directed, weight);
    }

    // Add a new vertex and add an edge from parent to child, using array indices
    // instead of pointers
    void add_node(T data, unsigned int parent, bool directed=true, double weight=0.0)
    {
        // Convert the array indices into pointers
        Node<T>* parentPtr = &this->nodes[parent];

        // Call the usual function to add the vertex and edge
        add_edge(data, parentPtr, directed, weight);
    }
};

#endif /* GRAPH_HPP */
