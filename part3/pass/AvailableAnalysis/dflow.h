#ifndef __CLASSICAL_DATAFLOW_H__
#define __CLASSICAL_DATAFLOW_H__

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;


#undef DEBUG

#define DBG(a)
#endif

#define WIDTH 60

namespace llvm {

    typedef std::vector<BitVector> BitVectorList;
    typedef std::vector<BasicBlock*> BasicBlockList;
	
    
    struct TfOutput {
        BitVector element;
        std::map<BasicBlock*, BitVector> nvals;
    };

    struct BlkResult {
        BitVector in;
        BitVector out;

        TfOutput tfout;
    };

    enum Direction {
        INVALID_DIRECTION,
        FORWARD,
        BACKWARD
    };

    enum Meet {
        INVALID_MEETOP,
        UNION,
        INTERSECTION
    };

    struct DFResult {
       
        std::map<BasicBlock*, BlkResult> result;

        
        std::map<void*, int> domainToIndex;

        bool modified;
    };

    
    class Dflow {
    public:

    Dflow(Direction direction, Meet meetup_op)
        : direction(direction), meetup_op(meetup_op)
        {
        }

        
        BitVector applyM(BitVectorList inputs);

      
        DFResult run(Function &F, std::vector<void*> domain,
                           BitVector boundaryCondition, BitVector initCondition);

    protected:
       
        virtual TfOutput transferFn(BitVector input,
                                          std::vector<void*> domain,
                                          std::map<void*, int> domainToIndex,
                                          BasicBlock* block) = 0;

    private:

        Direction direction;
        Meet meetup_op;
    };



  

    std::string printValue(Value* v);

    std::string printOut(std::vector<void*> domain, BitVector on, int mode, map<string,string> instvar);


    class Expression {
    public:
        Value * v1;
        Value * v2;
        string s1;
        string s2;
        Instruction::BinaryOps op;
        Expression (Instruction * I);
        bool operator== (const Expression &e2) const;
        bool operator< (const Expression &e2) const;
        std::string toString() const;
    };

    std::string getShortValueName(Value * v);

    
    void printOut(std::vector<Expression> * x);

}

