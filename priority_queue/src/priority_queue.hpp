#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T>
class Node{
public:
	Node *fa,*ls,*rs;
	T val;
	int dep;
	Node():fa(nullptr),ls(nullptr),rs(nullptr),dep(0){}
	Node(T _val):fa(nullptr),ls(nullptr),rs(nullptr),val(_val),dep(0){}
	Node(Node *other):fa(nullptr),ls(nullptr),rs(nullptr),val(other->val),dep(other->dep){}
};
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	int sz;
	Node<T> *root;
public:
	/**
	 * TODO constructors
	 */
	void DFS(Node<T> *x,const Node<T> *las){
		if(las->ls!=nullptr){
			Node<T> *g1=new Node<T>(las->ls);
			x->ls=g1,g1->fa=x;
			DFS(x->ls,las->ls);
		}
		if(las->rs!=nullptr){
			Node<T> *g2=new Node<T>(las->rs);
			x->rs=g2,g2->fa=x;
			DFS(x->rs,las->rs);
		}
	}
	void DFS2(Node<T> *x){
		// cout<<"$$"<<x->val<<endl;
		if(x->ls!=nullptr)DFS2(x->ls);
		if(x->rs!=nullptr)DFS2(x->rs);
		delete x;
	}
	priority_queue():sz(0),root(nullptr){}
	priority_queue(const priority_queue &other) {
		sz=other.sz;
		root=new Node<T>(other.root);
		DFS(root,other.root);
	}
	/**
	 * TODO deconstructor
	 */
	void Delete(){
		if(sz)sz=0,DFS2(root);
	}
	~priority_queue() {
		Delete();
	}
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if(this==&other)return *this;
		Delete();
		sz=other.sz;
		root=new Node<T>(other.root);
		DFS(root,other.root);
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		if(!sz)throw container_is_empty();
		return root->val;
	}

	/**
	 * merge two priority_queues with at least O(logn) complexity.
	 * clear the other priority_queue.
	 */
	void update(Node<T> *x){
		if(x->ls==nullptr||x->ls->dep<x->rs->dep)std::swap(x->ls,x->rs);
		x->dep=(x->rs==nullptr)?0:x->rs->dep+1;
	}
	Node<T>* m_merge(Node<T> *x,Node<T> *y){
		if(x==nullptr)return y;
		if(y==nullptr)return x;
		if(Compare()(y->val,x->val)){
			x->rs=m_merge(x->rs,y);
			update(x);
			return x;
		}
		else {
			y->rs=m_merge(x,y->rs);
			update(y);
			return y;
		}
	}
	void merge(priority_queue &other) {
		root=m_merge(root,other.root);
		sz=sz+other.sz;
		other.root=nullptr,other.sz=0;
	}

	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		Node<T> *x=new Node<T>(e);
		root=m_merge(root,x);
		sz++;
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if(!sz)throw container_is_empty();
		Node<T> *x=root->ls,*y=root->rs;
		delete root;
		sz--;
		root=m_merge(x,y);
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
		return sz;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		return sz==0;
	}
};

}

#endif
