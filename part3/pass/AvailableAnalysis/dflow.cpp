#include "dflow.h"

/*
DataFlow Code*/
using namespace std;

namespace llvm {
map<string,string> instVar;
   
         vector<string>dupCheck;
    BitVector Dflow::applyM(BitVectorList inputs) {
        BitVector result;

        if (!inputs.empty()) {
            for (int i = 0; i < inputs.size(); i++) {
                if (i == 0) {
                    result = inputs[i];
                }
                else {
                    if (meetup_op == Meet::UNION) {
                        result |= inputs[i];
                    }
                    else if (meetup_op == Meet::INTERSECTION) {
                        result &= inputs[i];
                    }
                    else{
                        outs() << "Unknown Meetop \n";
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        return result;
    }

    /* Apply analysis on Function F */
    DFResult Dflow::run(Function &F, std::vector<void*> domain,
                                 BitVector boundaryCondition, BitVector initCondition) {
        std::map<BasicBlock*, BlkResult> result;
        bool modified = false;

        //DBG(outs() << "APPLY ANALYSIS :: \n");

        // Map domain values to index in bitvector
        std::map<void*, int> domainToIndex;
        for (int i = 0; i < domain.size(); i++)
            domainToIndex[(void*)domain[i]] = i;

        // Find Boundary Blocks
        BasicBlockList boundaryBlocks;
        switch (direction) {

        case Direction::FORWARD:
            // Entry = first block
            boundaryBlocks.push_back(&F.front());
            break;

        case Direction::BACKWARD:
            // TODO: Improve this. What if the function has exit(0)? Or use CFGNode from LLVM?
            // Exit = blocks with a return statement (could be multiple)
            //for(Function::iterator I = F.begin(), E = F.end(); I != E; ++I)
              for(auto &I:F)
                if (isa<ReturnInst>(I.getTerminator()))
                    boundaryBlocks.push_back(&I);
            break;

        default:
            errs() << "Invalid direction \n";
            break;
        }

       

       
        BlkResult boundaryRes = BlkResult();
     
        BitVector* value = (direction == Direction::FORWARD) ? &boundaryRes.in : &boundaryRes.out;

        *value = boundaryCondition;
        boundaryRes.tfout.element = boundaryCondition;
        for (BasicBlockList::iterator I = boundaryBlocks.begin(), E = boundaryBlocks.end(); I != E; ++I) {
            result[*I] = boundaryRes;	
        }

   
        BlkResult intRes = BlkResult();
        
        BitVector* intVal = (direction == Direction::FORWARD) ? &intRes.out : &intRes.in;

        *intVal = initCondition;
        intRes.tfout.element = initCondition;
      
        for(auto &BB:F){
        
            if (std::find(boundaryBlocks.begin(),boundaryBlocks.end(),(BasicBlock*)&BB) == boundaryBlocks.end()) {
              
                result[(BasicBlock*)&BB] = intRes;
            }
        }

  
        std::map<BasicBlock*, BasicBlockList > blockN;

  
         for(auto &BB:F){
            BasicBlockList nList;

            switch (direction) {
            case Direction::FORWARD:
                for (pred_iterator neighbor = pred_begin(&BB), E = pred_end(&BB); neighbor != E; ++neighbor)
                    nList.push_back(*neighbor);

                break;

            case Direction::BACKWARD:
                for (succ_iterator neighbor = succ_begin(&BB), E = succ_end(&BB); neighbor != E; ++neighbor)
                    nList.push_back(*neighbor);
                break;

            default:
                errs() << "Invalid direction \n";
                break;
            }

            blockN[&BB] = nList;

        }

     
        BasicBlockList tOrder;

        switch (direction) {

        case Direction::FORWARD: {
            ReversePostOrderTraversal<Function*> TR(&F);
            for (ReversePostOrderTraversal<Function*>::rpo_iterator I = TR.begin(), E = TR.end(); I != E; ++I) {
                tOrder.push_back(*I);
            }
        }
            break;

        case Direction::BACKWARD: {
            for (po_iterator<BasicBlock*> I = po_begin(&F.getEntryBlock()), E = po_end(&F.getEntryBlock()); I != E; ++I) {
                tOrder.push_back(*I);
            }
        }
            break;

        default:
            errs() << "Invalid direction \n";
            break;
        }

       
        bool conv = false;
        while (!conv) {
            conv = true;

            for (BasicBlockList::iterator BB = tOrder.begin(), BE = tOrder.end(); BB != BE; ++BB) {

                
                BlkResult& blkRes = result[*BB];

                
                BlkResult oldBlkRes = blkRes;
                BitVector oldVal = (direction == Direction::FORWARD) ? blkRes.out : blkRes.in;

                
                BitVectorList meetInputs;

                for (BasicBlockList::iterator NI = blockN[*BB].begin(), NE = blockN[*BB].end(); NI != NE; ++NI) {
                    BlkResult& nRes = result[*NI];
                    BitVector neighVal = nRes.tfout.element;

                  
                    std::map<BasicBlock*, BitVector>::iterator match = nRes.tfout.nvals.find(*BB);
                    if (match != nRes.tfout.nvals.end()) {
                        neighVal |= match->second;
                    }

                    meetInputs.push_back(neighVal);
                }

                
                BitVector* blkIn = (direction == Direction::FORWARD) ? &blkRes.in : &blkRes.out;
                if (!meetInputs.empty())
                    *blkIn = applyM(meetInputs);

             
                blkRes.tfout = transferFn(*blkIn, domain, domainToIndex, *BB);
            
                BitVector* blkOut = (direction == Direction::FORWARD) ? &blkRes.out : &blkRes.in;
                *blkOut = blkRes.tfout.element;

                if (conv) {
                  
                    if (*blkOut != oldVal ||
                        blkRes.tfout.nvals.size() != oldBlkRes.tfout.nvals.size())
                        conv = false;
                }
            }
        }

      
        DFResult output;
        output.domainToIndex = domainToIndex;
        output.result = result;
        output.modified = modified;

        return output;
    }
    std::string *getValueName(Use *it){
        std::string *op = new std::string;
        if ((*it)->hasName()){
            *op = (*it)->getName().data();
        }
        else {
            return NULL;
        }
        return op;
    }

 
    Expression::Expression (Instruction * I) {
        if (BinaryOperator * BO = dyn_cast<BinaryOperator>(I)) {
            this->v1 = BO->getOperand(0);
            this->v2 = BO->getOperand(1);
            this->s1 = BO->getOperand(0)->getName().data();
            this->s2 = BO->getOperand(1)->getName().data();
            //errs()<<"t1 "<<BO->getOperand(0)->getName()<<"  "<<BO->getOperand(1)->getName().data()<<"\n";
           // if (BO->getOpcode()==Instruction::Load)
            //{
           // errs()<<"l1 "<<"\n";
            //} 
            
            this->op = BO->getOpcode();
        }
        else {
            errs() << "We're only considering BinaryOperators\n";
        }
    }

    
 
    bool Expression::operator== (const Expression &e2) const {
        return this->v1 == e2.v1 && this->v2 == e2.v2 && this->op == e2.op;
    }

  
    bool Expression::operator< (const Expression &e2) const {
        if (this->v1 == e2.v1)
            if (this->v2 == e2.v2)
                if (this->op == e2.op)
                    return false;
                else
                    return this->op < e2.op;
            else
                return this->v2 < e2.v2;
        else
            return this->v1 < e2.v1;
    }

 
    std::string Expression::toString() const {
        std::string op = "?";
        switch (this->op) {
        case Instruction::Add:
        case Instruction::FAdd: op = "+"; break;
        case Instruction::Sub:
        case Instruction::FSub: op = "-"; break;
        case Instruction::Mul:
        case Instruction::FMul: op = "*"; break;
        case Instruction::UDiv:
        case Instruction::FDiv:
        case Instruction::SDiv: op = "/"; break;
        case Instruction::URem:
        case Instruction::FRem:
        case Instruction::SRem: op = "%"; break;
        case Instruction::Shl: op = "<<"; break;
        case Instruction::AShr:
        case Instruction::LShr: op = ">>"; break;
        case Instruction::And: op = "&"; break;
        case Instruction::Or: op = "|"; break;
        case Instruction::Xor: op = "xor"; break;
        default: op = "op"; break;
        }
       // return getShortValueName(v1) +  op +  getShortValueName(v2);
        if(std::find(dupCheck.begin(),dupCheck.end(),getShortValueName(v1) + " " + op + " " + getShortValueName(v2))==dupCheck.end()){
        dupCheck.push_back(getShortValueName(v1) + " " + op + " " + getShortValueName(v2));
        string finalOut = "";
        //errs()<<std::isdigit(getShortValueName(v1)[0])<<"\n";
        if((getShortValueName(v1).compare(getShortValueName(v2))>0)&&(!std::isdigit(getShortValueName(v1)[0])&&!std::isdigit(getShortValueName(v2)[0])))
        {

         return getShortValueName(v2) +  op +  getShortValueName(v1);
        }
        else
        {
   return getShortValueName(v1) +  op +  getShortValueName(v2);
        }
        
        }
        else
        {
        return "";
        }
    }

    // Silly code to print out a set of expressions in a nice
    // format
    // void printOut(std::vector<Expression> * x) {
    //     bool first = true;
    //     outs() << "{";
    //     for (std::vector<Expression>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it) 
    //     {
    //         if (!first) {
    //             outs() << ", ";
    //         }
    //         else { first = false; }
          
    //         outs() << ("a" + it->s1);
    //     }
    //     outs() << "}\n";
    // }



    std::string getShortValueName(Value * v) {
        if (v->getName().str().length() > 0) {
        	return instVar["%"+v->getName().str()];	
         //   return "%" + v->getName().str();
        }
        else if (isa<Instruction>(v)) {
            std::string s = "";
            raw_string_ostream * strm = new raw_string_ostream(s);
            v->print(*strm);
            std::string inst = strm->str();
            //errs()<<"inst "<<inst<<"\n";
            size_t idx1 = inst.find("%");
            size_t idx2 = inst.find(" ",idx1);
            if (idx1 != std::string::npos && idx2 != std::string::npos) {
            //errs()<<inst.substr(idx1,idx2-idx1)<<" "<<instVar["%10"]<<"\n";
               return instVar[inst.substr(idx1,idx2-idx1)];	
               // return inst.substr(idx1,idx2-idx1);
            }
            else {
                return "";
                //return "\"" + inst + "\"";
            }
        }
        else if (ConstantInt * cint = dyn_cast<ConstantInt>(v)) {
            std::string s = "";
            raw_string_ostream * strm = new raw_string_ostream(s);
            cint->getValue().print(*strm,true);
            return strm->str();
        }
        else {
            std::string s = "";
            raw_string_ostream * strm = new raw_string_ostream(s);
            v->print(*strm);
            std::string inst = strm->str();
            return "\"" + inst + "\"";
        }
    }



    // void printBitVector(BitVector b)
    // {
    //     unsigned int i;
    //     unsigned int b_size = b.size();

    //     if(b_size == 0)
    //         errs() << "-";
    //     else
    //     {
    //         for(i = 0; i < b.size() ; i++)
    //         {
    //             if(b[i] == true)
    //                 errs() << "1";
    //             else
    //                 errs() << "0";
    //         }
    //     }
    //     errs() << "\n";
    // }

    // void printResult(DFResult output)
    // {
    //     for(auto entry : output.result)
    //     {
    //         DBG(outs() << "BB " << entry.first->getName() << "\n");

    //         printBitVector(entry.second.in);
    //         printBitVector(entry.second.out);
    //     }
    // }

    std::string printVal(Value* v)
    {
        std::string res; llvm::raw_string_ostream raw_st(res);
        v->print(raw_st);
        return res;
    }

    std::string getShortValueName(Value * v);

    std::string printOut(std::vector<void*> domain, BitVector on, int  mode, map<string,string> instvar) {
        std::stringstream ss;
       
        int ind = 0;
        instVar=instvar;
        if(domain.size()>0)
        {
         ss<< "Available: ";
        }
    dupCheck.clear();
        for (int i = 0; i < domain.size(); i++) {
            
            if (on[i]) {
        //    errs()<<"on "<<on[i]<<" "<<domain[i]<<"\n";
                if (ind > 0)
                    ss << " ";

                switch(mode)
                {
                case 0:
                    // Value*
                    
                    ss << " " << getShortValueName((Value*)domain[i]);
                    break;

                case 1:
                    // Expression*
                    
                    ss << " " << ((Expression*)domain[i])->toString();
                    
                    
                    //ss<<"st "<<((Expression*)domain[i])->s1;
                    break;


                default:
                    errs() << "Invalid mode :: " << mode << "\n";
                    break;
                }


                ind++;
            }
        }

        ss << "\n";

        return ss.str();
    }


}
