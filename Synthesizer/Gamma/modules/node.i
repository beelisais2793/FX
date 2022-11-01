%{
#include "Gamma.Node.h"
%

namespace gam
{
    template <class T>
    class Node2{
    public:
        Node2();
        Node2(bool zeroLinks);
        ~Node2();
        
        T * nodeL;					///< Pointer to left node
        T * nodeR;					///< Pointer to right node
        
        void nodeInsertL(T& node);	///< Insert myself to left of node
        void nodeInsertR(T& node);	///< Insert myself to right of node
        void nodeRemove();			///< Remove myself from linked list

        /// Returns leftmost link
        const Node2<T>& leftmost() const;
        bool linked() const;

        void print(const char * append = "\n", FILE * fp = stdout) const;
        void printAll(const char * append = "\n", FILE * fp = stdout) const;
    };


    /// Triply-linked node
    template <class T>
    class Node3{
    public:

        T * parent;		///< Parent node
        T * child;		///< Child node
        T * sibling;	///< Right sibling

        Node3();
    
    
        void addFirstChild(T * newChild);
        void addLastChild(T * newChild);
        
        void removeFromParent();

        T * lastChild();
        T * next(const T * const terminal);
        const T * next(const T * const terminal) const;
        T * nextBreadth(const T * const terminal);
    };
}

