#include "dflow.h"

/*
Available Expression Code*/
using namespace llvm;

namespace {
    class AvailExpression : public FunctionPass {

    public:
        static char ID;
         std::map<string,string> instvar;

        AvailExpression() : FunctionPass(ID)
        {
            // Setup the pass
            Direction direction = Direction::FORWARD;
            Meet meet_op = Meet::INTERSECTION;

            pass = AvlAnalysis(direction, meet_op);
        }
        std::string getValueName(Use *it){
        std::string op ;//= new std::string;
        if ((*it)->hasName()){
            op = (*it)->getName().data();
            
        }
        else {
            return "";
        }
        return op;
    }
    static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


    private:

        
        class AvlAnalysis : public Dflow {
        public:
        std::map<string,string> instvar2;

            AvlAnalysis() : Dflow(Direction::INVALID_DIRECTION, Meet::INVALID_MEETOP) {	}
            AvlAnalysis(Direction direction, Meet meet_op) : Dflow(direction, meet_op) { }
std::string getValueName2(Use *it){
        std::string op ;//= new std::string;
        if ((*it)->hasName()){
            op = (*it)->getName().data();
            
        }
        else {
            return "";
        }
        return op;
    }
        protected:

            TfOutput transferFn(BitVector input, std::vector<void*> domain, std::map<void*, int> domIndex, BasicBlock* block)
            {
                TfOutput tfOut;

               
                int domSize = domIndex.size();
                BitVector GEN(domSize);
                BitVector KILL(domSize);

              
                for(auto &i:*block){
                    Instruction * I = &i;
                    if(I->getOpcode() == Instruction::Load){
		auto* ptr = dyn_cast<User>(&i);
		auto* op3 = dyn_cast<Value>(&i);
		std::string load;
		raw_string_ostream(load)<<(*op3);
		load.push_back('\0');
		int pos = load.find('%');
		
	//	errs() << "\tfs"<<": "<<load.substr(pos,2) << "\n";
		 for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) {
                 	 
			//errs()<<"load "<<getValueName2(it)<<" "<<load.substr(pos,2)<<"\n";
			instvar2.insert(pair<string,string>(getValueName2(it),load.substr(pos,2)));
			//errs()<<"load map "<<instvar2[getValueName2(it)]<<" \n";
			 
		}
		
		  
			//errs()<<"Val "<<I->getValueName()->first().str()<<"\n";
		
		}
                   
                    if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I)) {
                       
                        Expression *expr = new Expression(BI);
                        Expression *match = NULL;
                        bool found = false;

                        for(void* element : domain)
                        {
                            if((*expr) == *((Expression *) element))
                            {
                                found = true;
                                match = (Expression *) element;
                                break;
                            }
                        }

                        if(found)
                        {
                            int valInd = domIndex[(void*)match];

                        
                            GEN.set(valInd);
                        }
                    }

                    int inscode=I->getOpcode();
                    if(inscode)
                    {
                        
                        for(auto domain_itr = domain.begin() ; domain_itr != domain.end() ; domain_itr++)
                        {
                            Expression* expr = (Expression*) (*domain_itr);

                            //errs()<<"expr: "<<expr->toString()<<"\n";
                            //errs()<<"Expr: "<<expr->toString()<<"\nop"<<op1.data()<<" "<<"\nop2"<<op2.data()<<"\ninsn\n"<<insn.data()<<"\n";
                           
                             auto* op1 = dyn_cast<Value>(expr->v1);
                            auto* op2 = dyn_cast<Value>(expr->v2);
                            std::string opst1;
			    raw_string_ostream(opst1)<<(*op1);
			    opst1.push_back('\0');
		             int pos = opst1.find('%');
		             
		            string sto1="";
		              if(opst1.length()>= pos+4)
		              {sto1=opst1.substr(pos,2);
		             }
		          //  errs()<<"opst1: "<<opst1<<"\n";
		             std::string opst2="";
		              raw_string_ostream(opst2)<<(*op1);
		            //   pos=opst2.find('%');
		              if(opst2.find('%')!=string::npos)
                     { pos=opst2.find('%');}
                      else
                      {
                      pos=opst1.find("%");
                      opst2=opst1;
                      }
			   string sto2="";
			   if(opst2.length()>= pos+4)
		              {sto2=opst2.substr(pos,2);
		             }
                     
			   
		           // errs()<<"opst2: "<<opst2<<"\n";
				// errs()<<"expr: "<<expr->toString()<<"op "<<"\n";
				// errs()<<"Expr: "<<expr->toString()<<"\nop"<<op1<<" "<<"\nop2"<<op2<<"\ninsn\n"<<insn_str<<"\n";
				// errs()<<"Insn: "<<insn_str<<"\n";
				// errs()<<"sto1: "<<sto1<<"\n";
				string st="";
				//errs()<<"opcode: "<<I->getOpcode() <<" "<<Instruction::Store<<"  \n";
				if(I->getOpcode() == Instruction::Store){
				//errs()<<"sto2: "<<sto2<<"\n";
		auto* ptr1 = dyn_cast<User>(I);
		auto* op31 = dyn_cast<Value>(I);
		
		std::string store;
		raw_string_ostream(store)<<(*op31);
		store.push_back('\0');
		pos = store.find('%',15);
		  if(store.length()>=pos+4)
			st=store.substr(pos+1,1);
	//	errs()<<"stoo1ex: "<<store<<"\n";
        }
                                       // errs()<<"stex: "<<st<<" "<<instvar2[st]<<"a"<<sto1<<" "<<instvar2[st]<<" "<<sto2<<"\n";
		
                         
                           if((instvar2[st]==sto1||instvar2[st]==sto2) && (st!=""))
                            {
                                
                               //errs()<<"Expr4: "<<expr->toString()<<" "<<"\n";
                                
                                std::map<void*, int>::iterator iter = domIndex.find((void*) expr);

                                if (iter != domIndex.end())
                                {
                            
                                    KILL.set((*iter).second);
                                    GEN.reset((*iter).second);
                                }
                            }
                        }
                    }

                }

          

                tfOut.element = KILL;
                
                tfOut.element.flip();
               
                tfOut.element &= input;
                tfOut.element |= GEN;

                return tfOut;
            }

        };


        AvlAnalysis pass;

        virtual bool runOnFunction(Function &F) {
        if (F.getName().compare("main")==0){
            return false;
        }
            
            std::string function_name = F.getName().data();
            
            DFResult output;

           
            std::vector<void*> domain;

       

          
            for(auto &FI:F){
                BasicBlock* block = &FI;
                
                for(auto &i:*block){
                    Instruction * I = &i;
                      
		if(I->getOpcode() == Instruction::Load){
		auto* ptr = dyn_cast<User>(&i);
		auto* op3 = dyn_cast<Value>(&i);
		std::string load;
		raw_string_ostream(load)<<(*op3);
		load.push_back('\0');
		int pos = load.find('%');
		string sbstr=load.substr(pos,3);
        rtrim(sbstr);
		//errs() << "\tfs"<<": "<<load.substr(pos,3) << "\n";
		 for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) {
                 	 
			//errs()<<"load "<<getValueName(it)<<" ";
			instvar.insert(pair<string,string>(sbstr,getValueName(it)));
			//errs()<<"load map2 "<<instvar[load.substr(pos,2)]<<" \n";
			 
		}
		
		  
			//errs()<<"Val "<<I->getValueName()->first().str()<<"\n";
		
		}
		
		
                    
                    if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I)) {

                       
                        Expression *expr = new Expression(BI);
                        bool found = false;

                        for(void* element : domain)
                        {
                            if((*expr) == *((Expression *) element))
                            {
                                found = true;
                                break;
                            }
                        }

                        if(found == false){
                        
                            domain.push_back(expr);
                            }
                        else
                            delete expr;
                    }
                }
            }

          
            BitVector boundaryCondition(domain.size(), false);
            BitVector initCondition(domain.size(), true);


            output = pass.run(F, domain, boundaryCondition, initCondition);
           
            std::map<void*, int> domIndex;
            for (int i = 0; i < domain.size(); i++)
                domIndex[(void*)domain[i]] = i;

           
            std::stringstream ss;

            errs()<<"\nAvailExpression: "<<F.getName().data()<<"\n";
            for(auto &BI:F){
                BasicBlock* block = &BI;

                
                BitVector avlExprs = output.result[block].in;

               
                std::vector<std::string> out;
                
              
                ss.clear();
                ss << std::setw(WIDTH) << std::right;
                ss.str(std::string());
                errs()<<"----- "<<block->getName().data()<<"-----\n";
               
                

                
                for(auto &insn:*block) {

                   
                    Instruction * I = &insn;

                   
                    if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I)) {
                     
                        Expression *expr = new Expression(BI);
                        Expression *match = NULL;
                        bool found = false;

                        for(void* element : domain)
                        {
                            if((*expr) == *((Expression *) element))
                            {
                                found = true;
                                match = (Expression *) element;
                                break;
                            }
                        }

                  
                        if(found)
                        {
                            int valInd = domIndex[(void*)match];

                            
                            avlExprs.set(valInd);
                        }
                    }
// for(int i=0; i<avlExprs.size();i++)
//                                     {
//                                         errs()<<"avl entry "<<avlExprs[i]<<"\n";
//                                     }
                   
                    int ins_op  =  I->getOpcode();
                    StringRef inst_r =I->getName();

                    if(ins_op)
                    {
                       

                        for(auto domain_itr = domain.begin() ; domain_itr != domain.end() ; domain_itr++)
                        {
                            Expression* expr = (Expression*) (*domain_itr);

             
				// errs()<<"expr: "<<expr->toString()<<"op "<<"\n";
				// errs()<<"Expr: "<<expr->toString()<<"\nop"<<op1<<" "<<"\nop2"<<op2<<"\ninsn\n"<<insn_str<<"\n";
				// errs()<<"Insn: "<<insn_str<<"\n";

              
                    
               auto* op1 = dyn_cast<Value>(expr->v1);
                            auto* op2 = dyn_cast<Value>(expr->v2);
                            std::string opst1;
			    raw_string_ostream(opst1)<<(*op1);
			    opst1.push_back('\0');
		             int pos = opst1.find('%');
		             
		            string sto1="";
		              if(opst1.length()>= pos+4)
		              {sto1=opst1.substr(pos,2);
		             }
		       //     errs()<<"opst1in: "<<opst1<<"\n";
		             std::string opst2="";
		              raw_string_ostream(opst2)<<(*op2);
                    //  errs()<<"opst2in: "<<opst2<<" "<<*op2<<"\n";
		              if(opst2.find('%')!=string::npos)
                     { pos=opst2.find('%');}
                      else
                      {
                      pos=opst1.find("%");
                      opst2=opst1;
                      }
			   string sto2="";
			   if(opst2.length()>= pos+4)
		              {sto2=opst2.substr(pos,2);
		             }
			   
		            
				// errs()<<"expr: "<<expr->toString()<<"op "<<"\n";
				// errs()<<"Expr: "<<expr->toString()<<"\nop"<<op1<<" "<<"\nop2"<<op2<<"\ninsn\n"<<insn_str<<"\n";
				// errs()<<"Insn: "<<insn_str<<"\n";
				// errs()<<"sto1: "<<sto1<<"\n";
				string st="";
			//	errs()<<"opcode: "<<I->getOpcode() <<" "<<Instruction::Store<<"  \n";
				if(I->getOpcode() == Instruction::Store){
				//errs()<<"sto2: "<<sto2<<"\n";
	
		auto* op31 = dyn_cast<Value>(I);
		
		std::string store;
		raw_string_ostream(store)<<(*op31);
		store.push_back('\0');
		pos = store.find('%',13);
        	//errs()<<"stoo1: "<<store<<"\n";
		  if(store.length()>=pos+4)
			st=store.substr(pos+1,1);
	
        }
        //errs()<<"stp1: "<<instvar[sto1]<<"\n";
		
                            if(instvar[sto1]==st || instvar[sto2]==st)
                            {//errs()<<"st1: "<<st<<"\n";
                                

                               
                                
                                std::map<void*, int>::iterator iter = domIndex.find((void*) expr);

                                if (iter != domIndex.end())
                                {
                                    //errs()<< "Expr : " << expr->toString()  <<" "<<(*iter).second <<" \n";
                                    //DBG(outs() << "Index : " << (*iter).second  << "\n");
                                    avlExprs.reset((*iter).second);
                                    // for(int i=0; i<avlExprs.size();i++)
                                    // {
                                    //     errs()<<"avl "<<avlExprs[i]<<"\n";
                                    // }
                                }
                            }
                            //errs()<<"opcode2: "<<I->getOpcode() <<" "<<Instruction::Store<<"  \n";
                        }
                    }

       

                   
                    PHINode* phi = dyn_cast<PHINode>(&insn);
                  
                    
                    if(phi == NULL)
                    {
                   // errs()<<"p\n";
                   if(insn.getOpcode()==block->getTerminator()->getOpcode()){
                        ss.clear();
                      //  errs()<<"ps "<<insn.getOpcode()<<" "<<block->getTerminator()->getOpcode()<<"\n";
                        ss.str(std::string());
                        //ss << std::setw(WIDTH) << std::right;
                        // for(int i=0; i<avlExprs.size();i++)
                        //             {
                        //                 errs()<<"avl2 "<<avlExprs[i]<<"\n";
                        //             }
                        errs() << printOut(domain, avlExprs, 1,instvar);
                        out.push_back(ss.str());
                       }
                    }
                }

           
                for (std::vector<std::string>::iterator it = out.begin(); it != out.end(); ++it)
                    outs() << *it << "\n";

            }


            return false;
        }

        virtual bool runOnModule(Module& M) {
            bool modfy = false;

            for (Module::iterator mod = M.begin(), modE = M.end(); mod != modE; ++mod) {
                bool fm = false;

                fm = runOnFunction(*mod);

                modfy |= fm;
            }

            return modfy;
        }

    };

    char AvailExpression::ID = 0;
    RegisterPass<AvailExpression> X("AvailExpression",
                                         "15745 Available Expressions");
}
