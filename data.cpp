#include "data.h"
#include <QMessageBox>

Data::Data(QObject *parent) : QObject(parent)
{

}

Data::iterator Data::iteratorAt(int parentNodeIndex, int indexInNode)
{
	//judge overflow
	if (parentNodeIndex > nodeNum - 1){
		QMessageBox::warning(NULL, tr("Error"), tr("iteratorAt overflow 1"));
	}
	//get node
	auto p_node = firstNode;
	while (parentNodeIndex){
		p_node = p_node->nextNode();
		--parentNodeIndex;
	}
	//judge overflow
	if (indexInNode > p_node->charNum() - 1){
		QMessageBox::warning(NULL, tr("Error"), tr("iteratorAt overflow 2"));
	}
	auto p_heap = p_node->firstHeap();
	while (indexInNode > p_heap->charNum() - 1){
		indexInNode -= p_heap->charNum();
		p_heap = p_heap->nextHeap();
	}
	return iterator(p_node, p_heap, indexInNode);
}

Data::iterator Data::iteratorAt(int parentNodeIndex, int parentHeapIndex, int indexInHeap)
{
	//judge overflow
	if (parentNodeIndex > nodeNum - 1){
		QMessageBox::warning(NULL, tr("Error"), tr("iteratorAt overflow 1"));
	}
	//get node
	auto p_node = firstNode;
	while (parentNodeIndex){
		p_node = p_node->nextNode();
		--parentNodeIndex;
	}
	//judge overflow
	if (parentHeapIndex > p_node->heapNum() - 1){
		QMessageBox::warning(NULL, tr("Error"), tr("iteratorAt overflow 3"));
	}
	//get heap
	auto p_heap = p_node->firstHeap();
	while (parentHeapIndex > p_heap->charNum() - 1){
		parentHeapIndex -= p_heap->charNum();
		p_heap = p_heap->nextHeap();
	}
	//final check
	if (indexInHeap > p_heap->charNum() - 1){
		QMessageBox::warning(NULL, tr("Error"), tr("iteratorAt overflow 2"));
	}

	return iterator(p_node, p_heap, indexInHeap);
}

const Data::Node & Data::operator[](int n)
{
	if (n > nodeNum - 1 || n < 0){
		QMessageBox::warning(this->parent(), tr("Error"), tr("Try to get a non-existent Node"));
	} else {//n is legal
		auto p = firstNode;
		while (n){
			p = p->nextp;
			--n;
		}
		return *p;
	}
}


QChar Data::iterator::operator*() const
{
	return m_parentHeap->operator [](m_index);
}

Data::iterator &Data::iterator::operator++()
{
	++m_index;
	if (m_index == m_parentHeap->charNum() - 1){//not int this heap
		if (m_parentHeap->nextHeap()){//goto next heap
			m_parentHeap = m_parentHeap->nextHeap();
			m_index = 0;
		} else {//no next heap
			if (m_parentNode->nextNode()){//go next node
				m_parentNode = m_parentNode->nextNode();
				m_parentHeap = m_parentNode->firstHeap();
				m_index = 0;
			} else {//no next node
				overflow = true;
			}
		}
	}
	return *this;
}

const Data::iterator &Data::iterator::operator++(int)
{
	auto t = *this;
	this->operator ++();
	return t;
}

const Data::iterator &Data::iterator::operator--()
{
	--m_index;
	if (m_index < 0){//not in this heap
		if (m_parentHeap->preHeap()){//goto previous heap
			m_parentHeap = m_parentHeap->preHeap();
			m_index = m_parentHeap->charNum() - 1;
		} else {//no previous heap
			if (m_parentNode->preNode()){//goto previous node
				m_parentNode = m_parentNode->preNode();
				m_parentHeap = m_parentNode->lastHeap();
				m_index = m_parentHeap->charNum() - 1;
			} else {//no previous node
				overflow = true;
			}
		}
	}
	return *this;
}

const Data::iterator &Data::iterator::operator--(int)
{
	auto t = *this;
	this->operator --();
	return t;
}

const Data::iterator &Data::iterator::operator+(int n) const
{
	auto t = *this;
	while (n){
		++t;
		--n;
	}
	return t;
}

const Data::iterator &Data::iterator::operator-(int n) const
{
	auto t = *this;
	while (n){
		++t;
		--n;
	}
	return t;
}

bool Data::iterator::operator==(const Data::iterator & another) const
{
	return m_parentNode == another.m_parentNode && m_parentHeap == another.m_parentHeap && m_index == another.m_index;
}

int Data::iterator::operator-(const Data::iterator & another)
{
	int result = 0;
	auto t = *this;
	while (!(t.isOverFlow() || another == *this)){
		if ((*t).unicode() >= 0x4e00 && (*t).unicode <= 0x9fa5){
			//chinese char
			result +=2;
		} else {
			++result;
		}
		++t;
	}
	if (t.isOverFlow()) result = -1;
	return result;
}

Data::Heap *Data::Node::lastHeap()
{
	auto p = m_firstHeap;
	while (p->nextp){
		p = p->nextp;
	}
	return p;
}

QChar Data::Node::operator[](int n)
{
	if (n > m_charNum - 1){//not in this node
		/* ===== if not in this node, we shouldn't goto next node
		if (nextp){//go to next node
			return (*nextp)[n - charNum];
		} else {//no next node
			QMessageBox::warning(NULL, tr("Error"), tr("Try to find overflow char from Node"));
		}
		  =====*/
			QMessageBox::warning(NULL, tr("Error"), tr("Try to find overflow char from Node"));
	} else {//in this node
		/* ===== use a loop instead of resursion
		return (*m_firstHeap)[n];
		   =====*/
		auto heap = m_firstHeap;
		while (n > heap->charNum() - 1){
			n -= heap->charNum();
			heap = heap->nextHeap();
		}
		return heap->operator [](n);
	}
}

QChar Data::Heap::operator[](int n)
{
	if (n > num - 1){//not in this heap
		if (nextp){//goto next heap
			return (*nextp)[n - num];
		} else {//no next heap
			QMessageBox::warning(NULL, tr("Error"), tr("Try to find overflow char from Heap"));
		}
	} else {//in this heap
		return ch[n];
	}
}