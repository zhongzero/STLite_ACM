/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
	struct my_true_type{};
	struct my_false_type{};
	template<class T>
	struct my_type_traits{
		using iterator_assignable=typename T::iterator_assignable;
	};
	template<class Key,class T,class Compare = std::less<Key> > class map {
	public:
		typedef pair<const Key, T> value_type;
	private:
		enum Col{red,black};
		struct node{
			node *fa,*lson,*rson;
			value_type *G;
			Col col;
			int sz;
			node(Key _val,T _val2,node *tmp=nullptr):fa(tmp),lson(tmp),rson(tmp),col(black),sz(0){G=new value_type(_val,_val2);}
			node(const node &other){
				G=new value_type(*(other.G));
				col=other.col,sz=other.sz;
			}
			~node(){if(G!=nullptr)delete G;}
		};
		class Red_Black_Tree{
		public:
			node *root,*nodeNull;
			int fir;
			void Clear(node *x){
				if(x==nodeNull)return;
				Clear(x->lson),Clear(x->rson);
				delete x;
			}
			void Clear_nodeNull(){
				nodeNull->fa=nodeNull->lson=nodeNull->rson=nodeNull;
				nodeNull->col=black,nodeNull->sz=0;
			}
		public:
			Red_Black_Tree(){nodeNull=nullptr,root=nullptr,fir=1;}
			void init(const Key _val,T _val2){
				nodeNull=new node(_val,_val2,nullptr);
				Clear_nodeNull();
				root=nodeNull;
				fir=0;
			}
			void Build(node *x,const node *y,const node *other_nodeNull){
				if(y->lson!=other_nodeNull){
					x->lson=new node(*(y->lson));
					x->lson->fa=x->lson->lson=x->lson->rson=nodeNull;
					x->lson->fa=x;
					Build(x->lson,y->lson,other_nodeNull);
				}
				if(y->rson!=other_nodeNull){
					x->rson=new node(*(y->rson));
					x->rson->fa=x->rson->lson=x->rson->rson=nodeNull;
					x->rson->fa=x;
					Build(x->rson,y->rson,other_nodeNull);
				}
			}
			Red_Black_Tree(const Red_Black_Tree &other){
				if(other.nodeNull==nullptr)nodeNull=nullptr,root=nullptr,fir=1;
				else {
					fir=0;
					nodeNull=new node(*(other.nodeNull));
					Clear_nodeNull();
					root=new node(*(other.root));
					root->fa=root->lson=root->rson=nodeNull;
					Build(root,other.root,other.nodeNull);
				}
			}
			Red_Black_Tree & operator=(const Red_Black_Tree &other){
				if(this==&other)return *this;
				Clear(root),delete nodeNull;
				if(other.nodeNull==nullptr)nodeNull=nullptr,root=nullptr,fir=1;
				else {
					fir=0;
					nodeNull=new node(*(other.nodeNull));
					Clear_nodeNull();
					root=new node(*(other.root));
					root->fa=root->lson=root->rson=nodeNull;
					Build(root,other.root,other.nodeNull);
				}
				return *this;
			}
			~Red_Black_Tree(){Clear(root);delete nodeNull;}
		public:
			void pushup(node *x){
				if(x==nodeNull)return;
				x->sz=x->lson->sz+x->rson->sz+1;
			}
			void R_Rotate(node *x){
				node *y=x->lson;
				node *f=x->fa;
				y->fa=f;
				if(f==nodeNull)root=y;
				else f->lson==x?f->lson=y:f->rson=y;
				x->lson=y->rson;
				if(y->rson!=nodeNull)y->rson->fa=x;
				y->rson=x,x->fa=y;
				pushup(x),pushup(y);
			}
			void L_Rotate(node *x){
				node *y=x->rson;
				node *f=x->fa;
				y->fa=f;
				if(f==nodeNull)root=y;
				else f->lson==x?f->lson=y:f->rson=y;
				x->rson=y->lson;
				if(y->lson!=nodeNull)y->lson->fa=x;
				y->lson=x,x->fa=y;
				pushup(x),pushup(y);
			}
		public:
			node* Insert(const Key val,T val2){
				if(fir)init(val,val2);
				node *x=root,*y=nodeNull;
				while(x!=nodeNull){
					x->sz++;
					y=x;
					// if(x.val==val)return pair<node*,bool>(x,0);
					// x=(val<(x->G)->first)?x->lson:x->rson;
					// if( !Compare()(val,(x->G)->first) && !Compare()((x->G)->first,val) )return pair<node*,bool>(x,0);
					x=(Compare()(val,(x->G)->first))?x->lson:x->rson;
				}
				node *z=new node(val,val2,nodeNull);
				z->sz=1;
				if(y==nodeNull){
					root=z;
					z->col=black;
				}
				else {
					// z->fa=y,val<(y->G)->first?y->lson=z:y->rson=z;
					z->fa=y,Compare()(val,(y->G)->first)?y->lson=z:y->rson=z;
					z->col=red;
				}
				Insert_Fixup(z);//????????????????????????????????????????????????????????????
				return z;
			}
			void Insert_Fixup(node *x){
				while(x->fa->col==red){
					if(x->fa==x->fa->fa->lson){
						// cout<<"$1"<<endl;
						node *y=x->fa->fa->rson;
						if(y->col==red){//????????????????????????
							x->fa->col=black,y->col=black;
							x->fa->fa->col=red;
							x=x->fa->fa;
							//??????????????????????????????
						}
						else { 
							if(x==x->fa->rson){//LR
								x=x->fa;
								L_Rotate(x);//???????????????LR?????????LL??????????????????????????????x??????????????????????????????
							}
							//LL
							x->fa->col=black,x->fa->fa->col=red;
							R_Rotate(x->fa->fa);//??????????????????????????????
							//?????????LL??????LR???????????????????????????????????????????????????????????????????????????break??????
						}
					}
					else {//swap(l,r)
						// cout<<"$2"<<endl;
						node *y=x->fa->fa->lson;
						if(y->col==red){//????????????????????????
							x->fa->col=black,y->col=black;
							x->fa->fa->col=red;
							x=x->fa->fa;
							//??????????????????????????????
						}
						else {
							if(x==x->fa->lson){//RL
								x=x->fa;
								R_Rotate(x);//???????????????RL?????????RR??????????????????????????????x??????????????????????????????
							}
							//RR
							x->fa->col=black,x->fa->fa->col=red;
							L_Rotate(x->fa->fa);//??????????????????????????????
							//?????????RR??????RL???????????????????????????????????????????????????????????????????????????break??????
						}
					}
				}
				root->col=black;
			}
			node* successor(node *x){
				x=x->rson;
				while(x->lson!=nodeNull)x=x->lson;
				return x;
			}
			void update_sz(node *x){
				if(x==nodeNull)return;
				while(x!=nodeNull)x->sz--,x=x->fa;
			}
			void swap_id_col_sz(node *&x,node *&y){
				if(x->fa!=nodeNull&&x->fa!=y) x->fa->lson==x?x->fa->lson=y:x->fa->rson=y;
				if(x->lson!=nodeNull&&x->lson!=y) x->lson->fa=y;
				if(x->rson!=nodeNull&&x->rson!=y) x->rson->fa=y;
				
				if(y->fa!=nodeNull&&y->fa!=x) y->fa->lson==y?y->fa->lson=x:y->fa->rson=x;
				if(y->lson!=nodeNull&&y->lson!=x) y->lson->fa=x;
				if(y->rson!=nodeNull&&y->rson!=x) y->rson->fa=x;

				if(x->lson==y)y->fa=x->fa,x->fa=y,x->lson=y->lson,y->lson=x,std::swap(x->rson,y->rson);
				else if(x->rson==y)y->fa=x->fa,x->fa=y,x->rson=y->rson,y->rson=x,std::swap(x->lson,y->lson);
				else if(y->lson==x)x->fa=y->fa,y->fa=x,y->lson=x->lson,x->lson=y,std::swap(y->rson,x->rson);
				else if(y->rson==x)x->fa=y->fa,y->fa=x,y->rson=x->rson,x->rson=y,std::swap(y->lson,x->lson);
				else std::swap(x->fa,y->fa),std::swap(x->lson,y->lson),std::swap(x->rson,y->rson);
				std::swap(x->col,y->col);
				std::swap(x->sz,y->sz);
				if(root==x)root=y;
				else if(root==y)root=x;
				std::swap(x,y);
			}
			void Delete(node *x){
				node *y,*z;
				if(x->lson==nodeNull||x->rson==nodeNull)y=x;
				else {
					y=successor(x);
					// delete x->G;
					// x->G=new value_type(*(y->G));
					swap_id_col_sz(x,y);
					//?????????x???????????????
				}
				//y??????????????????????????????y????????????????????????
				update_sz(y);
				z=(y->lson!=nodeNull)?y->lson:y->rson;
				z->fa=y->fa;
				if(y->fa==nodeNull)root=z;
				else (y->fa->lson==y)?y->fa->lson=z:y->fa->rson=z;
				if(y->col==black)Delete_Fixup(z);
				delete y;
				// if(flag)cout<<x->fa->G->first<<" "<<x->lson->G->first<<" "<<x->rson->G->first<<endl;
			}
			void Delete_Fixup(node *x){
				//??????x?????????????????????????????? ??????x??? ???+?????????????????????????????? x??? ???+??? or x??????,???????????????x?????? ??? ??????
				while(x!=root&&x->col==black){
					if(x->fa->lson==x){
						node *y=x->fa->rson;
						if(y->col==red){//Case1
							y->col=black,x->fa->col=red;
							L_Rotate(x->fa);
							y=x->fa->rson;
						}
						//??????Case1??????????????????Case2,3,4
						if(y->lson->col==black&&y->rson->col==black){//Case2
							y->col=red;
							x=x->fa;
						}
						else {
							if(y->rson->col==black){//Case3
								y->lson->col=black,y->col=red;
								R_Rotate(y);
								y=x->fa->rson;
							}
							//??????Case3??????????????????Case4
							//?????????Case4
							y->col=x->fa->col,x->fa->col=black,y->rson->col=black;
							L_Rotate(x->fa);
							x=root;
						}
					}
					else {//swap(l,r)
						node *y=x->fa->lson;
						if(y->col==red){//Case1
							y->col=black,x->fa->col=red;
							R_Rotate(x->fa);
							y=x->fa->lson;
						}
						//??????Case1??????????????????Case2,3,4
						if(y->rson->col==black&&y->lson->col==black){//Case2
							y->col=red;
							x=x->fa;
						}
						else {
							if(y->lson->col==black){//Case3
								y->rson->col=black,y->col=red;
								L_Rotate(y);
								y=x->fa->lson;
							}
							//??????Case3??????????????????Case4
							//?????????Case4
							y->col=x->fa->col,x->fa->col=black,y->lson->col=black;
							R_Rotate(x->fa);
							x=root;
						}
					}
				}
				x->col=black;
			}
			node* Find(const Key val) const{
				node *x=root;
				while(x!=nodeNull){
					// if(val==(x->G)->first)return x;
					// else if(val<(x->G)->first)x=x->lson;
					if(!Compare()(val,(x->G)->first)&&!Compare()((x->G)->first,val))return x;
					else if(Compare()(val,(x->G)->first))x=x->lson;
					else x=x->rson;
				}
				return nodeNull;
			}
			bool Is_Find(const Key val)const {
				node *x=root;
				while(x!=nodeNull){
					// if(val==(x->G)->first)return 1;
					// else if(val<(x->G)->first)x=x->lson;
					if(!Compare()(val,(x->G)->first)&&!Compare()((x->G)->first,val))return 1;
					else if(Compare()(val,(x->G)->first))x=x->lson;
					else x=x->rson;
				}
				return 0;
			}
			node* Find_pre(node *x) const{
				if(x==nodeNull){
					if(root==nodeNull)throw invalid_iterator();
					return Get_las();
				}
				if(x->lson!=nodeNull){
					x=x->lson;
					while(x->rson!=nodeNull)x=x->rson;
					return x;
				}
				else {
					while(x->fa!=nodeNull&&x->fa->lson==x)x=x->fa;
					if(x->fa==nodeNull)throw invalid_iterator();
					return x->fa;
				}
			}
			const node* Find_pre(const node *x) const{
				if(x==nodeNull){
					if(root==nodeNull)throw invalid_iterator();
					return Get_las();
				}
				if(x->lson!=nodeNull){
					x=x->lson;
					while(x->rson!=nodeNull)x=x->rson;
					return x;
				}
				else {
					while(x->fa!=nodeNull&&x->fa->lson==x)x=x->fa;
					return x->fa;
				}
			}
			node* Find_next(node *x)const {
				if(x==nodeNull)throw invalid_iterator();
				if(x->rson!=nodeNull){
					x=x->rson;
					while(x->lson!=nodeNull)x=x->lson;
					return x;
				}
				else {
					while(x->fa!=nodeNull&&x->fa->rson==x)x=x->fa;
					return x->fa;
				}
			}
			const node* Find_next(const node *x)const {
				if(x==nodeNull)throw invalid_iterator();
				if(x->rson!=nodeNull){
					x=x->rson;
					while(x->lson!=nodeNull)x=x->lson;
					return x;
				}
				else {
					while(x->fa!=nodeNull&&x->fa->rson==x)x=x->fa;
					return x->fa;
				}
			}
			node* Get_fir()const {
				if(root==nodeNull)return nodeNull;
				node *x=root;
				while(x->lson!=nodeNull)x=x->lson;
				return x;
			}
			node* Get_las()const {
				if(root==nodeNull)return nodeNull;
				node *x=root;
				while(x->rson!=nodeNull)x=x->rson;
				return x;
			}
			// void check(node* x){
			// 	if(x==nodeNull)return;
			// 	cout<<x->G->first<<" "<<x->G->second<<" "<<x->sz<<" "<<x->col<<endl;
			// 	if(x->lson!=nodeNull&&x->lson->fa!=x)cout<<"####"<<endl;
			// 	if(x->rson!=nodeNull&&x->rson->fa!=x)cout<<"####"<<endl;
			// 	check(x->lson),check(x->rson);
			// }
		};
		Red_Black_Tree Tree;
	public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		// The following code is written for the C++ type_traits library.
		// Type traits is a C++ feature for describing certain properties of a type.
		// For instance, for an iterator, iterator::value_type is the type that the 
		// iterator points to. 
		// STL algorithms and containers may use these type_traits (e.g. the following 
		// typedef) to work properly. 
		// See these websites for more information:
		// https://en.cppreference.com/w/cpp/header/type_traits
		// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
		// About iterator_category: https://en.cppreference.com/w/cpp/iterator
		// using difference_type = std::ptrdiff_t;
		// using value_type = T;
		// using pointer = T*;
		// using reference = T&;
		// using iterator_category = std::output_iterator_tag;
		// If you are interested in type_traits, toy_traits_test provides a place to 
		// practice. But the method used in that test is old and rarely used, so you
		// may explore on your own.
		// Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
		// this part is only for bonus.

		using iterator_assignable=my_true_type;

		node *p;
		const map<Key,T,Compare> *Belong;

		iterator() {
			p=nullptr;
			// TODO
		}
		iterator(node *_p,map<Key,T,Compare> *tmp){p=_p,Belong=tmp;}
		iterator(const iterator &other) {
			// cout<<"%"<<endl;
			p=other.p,Belong=other.Belong;
			// TODO
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			// cout<<"!!"<<endl;
			iterator Tmp=*this;
			// cout<<"!"<<endl;
			p=(Belong->Tree).Find_next(p);
			return Tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
			p=(Belong->Tree).Find_next(p);
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator Tmp=*this;
			p=(Belong->Tree).Find_pre(p);
			return Tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			p=(Belong->Tree).Find_pre(p);
			return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {return *(p->G);}
		bool operator==(const iterator &rhs) const {return p==rhs.p;}
		bool operator==(const const_iterator &rhs) const {return p==rhs.p;}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {return p!=rhs.p;}
		bool operator!=(const const_iterator &rhs) const {return p!=rhs.p;}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			// cout<<"###"<<endl;
			return p->G;
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
	private:
			// data members.
	public:
	
		using iterator_assignable=my_false_type;

		const node *p;
		const map<Key,T,Compare> *Belong;
		const_iterator() {p=nullptr;}
		const_iterator(node *_p,const map<Key,T,Compare> *tmp){p=_p,Belong=tmp;}
		const_iterator(const iterator &other) {p=other.p,Belong=other.Belong;}
		const_iterator(const const_iterator &other) {p=other.p,Belong=other.Belong;}
		const_iterator operator++(int) {
			const_iterator Tmp=*this;
			p=(Belong->Tree).Find_next(p);
			return Tmp;
		}
		const_iterator & operator++() {
			p=(Belong->Tree).Find_next(p);
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator Tmp=*this;
			p=(Belong->Tree).Find_pre(p);
			return Tmp;
		}
		const_iterator & operator--() {
			p=(Belong->Tree).Find_pre(p);
			return *this;
		}
		const value_type & operator*() const {return *(p->G);}
		bool operator==(const iterator &rhs) const {return p==rhs.p;}
		bool operator==(const const_iterator &rhs) const {return p==rhs.p;}
		bool operator!=(const iterator &rhs) const {return p!=rhs.p;}
		bool operator!=(const const_iterator &rhs) const {return p!=rhs.p;}
		const value_type* operator->() const noexcept {
			return p->G;
		}
	};
	/**
	 * TODO two constructors
	 */
	map() {}
	map(const map &other) :Tree(other.Tree){}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		if(this==&other)return *this;
		Tree=other.Tree;
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map() {
		clear();
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		node *GG=Tree.Find(key);
		if(GG==Tree.nodeNull)throw index_out_of_bound();
		return (GG->G)->second;
	}
	const T & at(const Key &key) const {
		node *GG=Tree.Find(key);
		if(GG==Tree.nodeNull)throw index_out_of_bound();
		return (GG->G)->second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		node *GG=Tree.Find(key);
		if(GG==Tree.nodeNull){
			insert(value_type(key,T()));
			GG=Tree.Find(key);
		}
		return (GG->G)->second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		node *GG=Tree.Find(key);
		if(GG==Tree.nodeNull)throw index_out_of_bound();
		return (GG->G)->second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		return iterator(Tree.Get_fir(),this);
	}
	const_iterator cbegin() const {
		return const_iterator(Tree.Get_fir(),this);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(Tree.nodeNull,this);
	}
	const_iterator cend() const {
		return const_iterator(Tree.nodeNull,this);
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		return size()==0;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		if(Tree.root==Tree.nodeNull)return 0;
		return Tree.root->sz;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		Tree.Clear(Tree.root),Tree.root=Tree.nodeNull;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		// std::cout<<"!!"<<value.first.val<<" "<<value.second<<std::endl;
		node *GG=Tree.Find(value.first);
		if(GG==Tree.nodeNull){
			node* G=Tree.Insert(value.first,value.second);
			return pair<iterator,bool> (iterator(G,this),1);
		}
		else return pair<iterator,bool> (iterator(GG,this),0);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if(pos==end()||pos.p==nullptr|| pos.Belong!=this )throw runtime_error();
		Tree.Delete(pos.p);
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		return Tree.Is_Find(key);
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		node *p=Tree.Find(key);
		if(p==Tree.nodeNull)return end();
		// cout<<"!!!"<<endl;
		return iterator(p,this);
	}
	const_iterator find(const Key &key) const {
		node *p=Tree.Find(key);
		if(p==Tree.nodeNull)return cend();
		return const_iterator(p,this);
	}
	// void Check(){
		// if(Tree.nodeNull->fa!=Tree.nodeNull){cout<<"!@@@!!!"<<endl;exit(0);}
		// if(Tree.root->fa!=Tree.nodeNull)cout<<"###"<<endl;
		// Tree.check(Tree.root);
	// }
};

}

#endif
