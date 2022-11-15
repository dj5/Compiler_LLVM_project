#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include <string>
#include <map>
#include <typeinfo>
#include "llvm/IR/CFG.h"
#include <list>
#include <iterator>

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "LivenessAnalysis"

using namespace llvm;

namespace lva{
struct Liveness : public FunctionPass {
  string func_name = "test";
  static char ID;
  Liveness() : FunctionPass(ID) {
//      GEN = new std::map<BasicBlock*, std::vector<std::string>*>;
  }

    std::map<BasicBlock*, std::vector<std::string>*> killSet;
    std::map<BasicBlock*, std::vector<std::string>*> GEN;
    std::map<BasicBlock*, std::vector<std::string>*> liveOut;
    std::list<BasicBlock*> worklist;



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


    void GENKill(BasicBlock* bb){
        std::pair<BasicBlock*,std::vector<std::string>*> *temp = new std::pair<BasicBlock*,std::vector<std::string>*>;
        auto killSet = new std::vector<std::string>;
        auto tempLB = new std::vector<std::string>;
        auto GEN = new std::vector<std::string>;
        std::vector<std::string>::iterator killSetIt;
        std::vector<std::string>::iterator GENIt;
       // errs() << "Block name: "<<bb->getName() << ":\n";
        for (auto& inst : *bb)
        {
           // errs() << "\t"  << inst << "\n";
          if(inst.getOpcode() == Instruction::Load){
            
            auto* ptr = dyn_cast<User>(&inst);
            auto* op3 = dyn_cast<Value>(&inst);
//errs() << "\t" << *ptr << "\n";
            int i = 1;
            for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) {
                auto var = getValueName(it);
                if(var!=NULL){
                    if(std::find(killSet->begin(), killSet->end(), *var) == killSet->end()) {
                      //  errs() <<i<< "GENKill:\t" << *(*it) << "\n";
                        GENIt = GEN->begin();
                        if (std::find(GEN->begin(),GEN->end(),*var)==GEN->end()){
                            GEN->insert(GENIt, *var);
                        }
                    }
                }
                i++;
            }
        
          }
          if(inst.getOpcode() == Instruction::Store){
           
            auto* ptr = dyn_cast<User>(&inst);
            auto* op3 = dyn_cast<Value>(&inst);
            //errs() << "\t" << *ptr << "\n";
            int i = 1;
            for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) {
                auto var = getValueName(it);
                if(var!=NULL){
                    if(std::find(killSet->begin(), killSet->end(), *var) == killSet->end()) {
//                        
                        killSetIt = killSet->begin();
                        if (std::find(killSet->begin(),killSet->end(),*var)==killSet->end()){
                            killSet->insert(killSetIt, *var);
                        }
                    }
                }
                i++;
            }
         
          }
          if (inst.isBinaryOp() || inst.getOpcode()==Instruction::PHI || inst.getOpcode()==Instruction::ICmp)
          {
            //  errs() << "inst:\t"  << inst <<  " is binary" << "\n";
//           
          }
        }

        temp->first = bb;
        temp->second = killSet;
        this->killSet.insert(*temp);
        temp->first = bb;
        temp->second = GEN;
        this->GEN.insert(*temp);
        // errs()<<"Block GEN"<<"\n";
        // for(auto it=GEN->begin();it!=GEN->end();it++)
        // {
        //   errs()<<"G: "<<(*it)<<",";
        // }
        // errs()<<"\n";
        //  errs()<<"Block KILL"<<"\n";
        // for(auto it=killSet->begin();it!=killSet->end();it++)
        // {
        //   errs()<<"K: "<<(*it)<<",";
        // }
        // errs()<<"\n";
    }


    bool LiveoutTransferFunc(BasicBlock* basic_block) {

        std::vector<std::string> *liveB;
        auto search = liveOut.find(basic_block);
        if ( search != liveOut.end()){
            liveB = search->second;
        }
        else {
            liveB = new std::vector<std::string>;
        }
        
       auto tempLB = new std::vector<std::string>;
      //  std::vector<std::string> *tempLB;
      
//        errs() << "--------------\n";
        for (BasicBlock *Succ : successors(basic_block)) {
            std::vector<std::string> *Succ1;
            std::vector<std::string> *Succ2;
            std::vector<std::string> *Succ3;
            std::vector<std::string> diff;
            // std::vector<std::string> *temp2;
            // std::vector<std::string> *temp3;
            std::vector<std::string>::iterator it,i1;
            std::map<BasicBlock*, std::vector<std::string>*>::iterator srch;

            srch = killSet.find(Succ);
            if (srch != killSet.end()){
                Succ2 = srch->second;
            }
            else {
               
                return false;
            }
            srch = GEN.find(Succ);
            if (srch != GEN.end()){
                Succ3 = srch->second;
            }
            else {
               
                return false;
            }
            srch = liveOut.find(Succ);
            if (srch != liveOut.end()){
                Succ1 = srch->second;
                Difference(Succ1,Succ2,&diff);
              //  it= std::set_difference(Succ1.begin(),Succ1.end(),Succ2.begin(),Succ2.end(),diff.begin());
            }


            auto temp2 = new std::vector<std::string>;
        //     errs() << " diff "<< "\n";
        //     for (i1=diff.begin();i1!=it;i1++){
        //     errs() << " "<< *i1 << "\n";
        // }

           
          

            Union(&diff,Succ3,temp2);
// std::set_union(diff.begin(),diff.end(),Succ3.begin(),Succ3.end(),temp2->begin());
            auto temp3 = new std::vector<std::string>;

            Union(temp2,tempLB,temp3);
// std::set_union(temp2->begin(),temp2->end(),tempLB->begin(),tempLB->end(),temp3->begin());
        
            tempLB = temp3;



        }
        bool chng = false;
        if (tempLB->size()!=liveB->size() || search==liveOut.end()){
            chng =true;
        }
        else {
            std::vector<std::string>::iterator it;
            for (it = liveB->begin();it != liveB->end();it++){

                //getValueName()
                if(std::find(tempLB->begin(),tempLB->end(),*it)==tempLB->end()){ //maybe using set_symmetric_difference instead
                    chng = true;
                    break;
                }
            }
        }
        if (chng){
            if(search!=liveOut.end()){
                search->second = tempLB;
            }
            else{
                auto temp = new std::pair<BasicBlock*,std::vector<std::string>*>;
                // errs()<<tempLB<<"tb\n";
                temp->first = basic_block;
                temp->second = tempLB;
                this->liveOut.insert(*temp);
            }

        }
        return chng;
    }

  
    std::list<BasicBlock*>* Traverse(BasicBlock* bb, std::list<BasicBlock*> *visited, std::list<BasicBlock*> *order) {
        visited->push_back(bb);
        for (BasicBlock *succ : successors(bb)) { 
            if (std::find(visited->begin(), visited->end(), succ)==visited->end()){
                visited = Traverse(succ,visited,order);
            }
        }
        order->push_back(bb);
        return visited;
    }

    void LivenessAnalysis(BasicBlock* bb) {
        bool chng = LiveoutTransferFunc(bb);
        if(chng){
            for (BasicBlock *pred : predecessors(bb)) {
                auto el = std::find(worklist.begin(), worklist.end(), pred);
                if(el==worklist.end()){
                    worklist.push_back(pred);
                }
            }
        }
    }

    void printLiveOut(std::map<BasicBlock*, std::vector<std::string>*> *inp){
    
        for (std::map<BasicBlock*, std::vector<std::string>*>::iterator block = inp->begin();block != inp->end();block++){
            errs() << block->first->getName() << ":\n\t";
            string s="";
            // sort(block->second->begin(),block->second->end(),compareStrings);
            for(auto var = block->second->begin(); var!=block->second->end();var++){
                // errs() << *var << "";
                s=s+string(var->c_str());
            }
           std::sort(s.begin(),s.end());
           errs()<<s<<"\n";
            errs() << "\n";
        }
        
    }

   

    void Difference(std::vector<std::string> *s1,std::vector<std::string> *s2,std::vector<std::string> *s3){
      //  std::set_difference(s1->begin(),s1->end(),s2->begin(),s2->end(),s3->begin());
        for (auto it=s1->begin();it!=s1->end();it++){
            if(std::find(s2->begin(),s2->end(),*it)==s2->end()){
                s3->insert(s3->end(),*it);
            }
        }
    }

    void Union(std::vector<std::string> *s1,std::vector<std::string> *s2,std::vector<std::string> *s3){
        // std::set_union(s1->begin(),s1->end(),s2->begin(),s2->end(),s3->begin());
        *s3 = *s1;

        for (auto it=s2->begin();it!=s2->end();it++){
            if(std::find(s1->begin(),s1->end(),*it)==s1->end()){

                s3->insert(s3->end(),*it);
            }
        }
    }

   

  bool runOnFunction(Function &F) override {
        if (F.getName().compare("main")==0){
            return false;
        }
        else{
            errs() << "--" << F.getName() << "--\n";
        }
      killSet.clear();
      GEN.clear();
      liveOut.clear();
      worklist.clear();
      // initialize GENKill and the Killset on all blocks
      for (auto& basic_block : F)
      {
          GENKill(&basic_block);
      }
      BasicBlock &basicBlock = F.getEntryBlock();
      std::list<BasicBlock*> *visited = new std::list<BasicBlock*>;
      auto visitedBlocks = Traverse(&basicBlock,visited,&worklist);

      

      //worklist.push_back(&basicBlock); 
      int i = 0;
      while (!worklist.empty()) 
      {
          auto block = worklist.front();
          LivenessAnalysis(block);
        //  printLiveOut(&liveOut);
          worklist.pop_front();
          i++;
      }
      printLiveOut(&liveOut);
      return false;
  }

}; 
}  

char lva::Liveness::ID = 0;
static RegisterPass<lva::Liveness> X("LivenessAnalysis", "LivenessAnalysis Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
