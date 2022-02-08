#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
using namespace std;

using namespace llvm;

namespace
{

  void printHashTable(unordered_map<string, int> hashTable)
  {
    errs() << "Hash Table\n";
    unordered_map<string, int>::iterator it;

    for (it = hashTable.begin(); it != hashTable.end(); ++it)
    {
      errs() << it->first << " -> " << it->second << "\n";
    }
  }

  // This method implements what the pass does
  void visitor(Function &F)
  {

    // Here goes what you want to do with a pass

    string func_name = "main";
    errs() << "ValueNumbering: " << F.getName() << "\n";
    int count = 0;

    // Comment this line
    // if (F.getName() != func_name) return;

    // errs() << "Testing 1...." << "\n";

    for (auto &basic_block : F)
    {
      unordered_map<string, int> hashTable;
      for (auto &inst : basic_block)
      {
        // errs() << inst << "\n";
        if (inst.getOpcode() == Instruction::Load)
        {
          // errs() << "This is Load"
          //        << "\n";

          string operand1 = inst.getOperand(0)->getName().str();
          // errs() << operand1 << "\n";

          int resultNum;

          auto key1 = hashTable.find(operand1);
          if (key1 == hashTable.end())
          {
            count++;
            hashTable.insert(make_pair(operand1, count));
            resultNum = count;
          }
          else
          {
            resultNum = key1->second;
          }

          // we can't get the result of an instruction
          // but we can get the address of the result in load instruction
          // and it remains same, so we are keepin the value of the load instruction result
          // in the hash table. The value address is changed to string
          Value *v = &inst;
          stringstream ss;
          ss << v;
          string value = ss.str();
          auto resultKey = hashTable.find(value);
          if (resultKey != hashTable.end())
          {
            hashTable.erase(resultKey);
          }
          hashTable.insert(make_pair(value, resultNum));
          // errs() << "value: " << value << "\n";
          // result print
          errs() << inst << "\t" << resultNum << " = " << resultNum << "\n";
        }
        if (inst.getOpcode() == Instruction::Store)
        {
          // errs() << "This is Store:"
          //        << "\n";
          // find operand 1 and 2 from the store instruction
          // insert the operand2 in hashTable if it's not available
          // if available get the value
          // overwrite the operand1 in hashTable
          string operand1 = inst.getOperand(0)->getName().str();
          string operand2 = inst.getOperand(1)->getName().str();
          Value *checkVal = inst.getOperand(0);
          if (llvm::ConstantInt *constInteger = dyn_cast<llvm::ConstantInt>(checkVal))
          {
            // if operand1 is a constant integer than the operand1 is overwritten here by the integer value
            // errs() << "Found constInt with " << operand2 << ": " << operand1 << constInteger << "\n";
            if (constInteger->getBitWidth() <= 32)
            {
              int constIntegerVal = constInteger->getSExtValue();
              operand1 = to_string(constIntegerVal);
              // errs() << "Found constInt with " << operand2 << ": str " << operand1 << " int " << constIntegerVal << "\n";
            }
          }
  
          // errs() << "value: " << operand1 << "::" << inst.getOperand(0) << "\n";
          auto key1 = hashTable.find(operand1);
          int operand2Value = 0;
          if (key1 == hashTable.end())
          {
            // we check if the value address of the operand1 is present in the hash table
            stringstream ss;
            ss << inst.getOperand(0);
            string value1 = ss.str();
            auto valKey = hashTable.find(value1);
            if (valKey == hashTable.end())
            {
              count++;
              hashTable.insert(make_pair(operand1, count));
              operand2Value = count;
            }
            else
            {
              operand2Value = valKey->second;
            }
          }
          else
          {
            operand2Value = key1->second;
          }

          auto key2 = hashTable.find(operand2);
          if (key2 != hashTable.end())
          {
            hashTable.erase(key2);
          }
          hashTable.insert(make_pair(operand2, operand2Value));

          // print the instruction with hash table value
          errs() << inst << "\t" << operand2Value << " = " << operand2Value << "\n";
        }
        if (inst.isBinaryOp())
        {
          // errs() << "Op Code:" << inst.getOpcodeName() << "\n";
          if (inst.getOpcode() == Instruction::Add)
          {
            // errs() << "This is Addition"
            //        << "\n";
          }
          if (inst.getOpcode() == Instruction::Sub)
          {
            // errs() << "This is Subtraction"
            //        << "\n";
          }
          if (inst.getOpcode() == Instruction::Load)
          {
            // errs() << "This is Load2"
            //        << "\n";
          }
          if (inst.getOpcode() == Instruction::UDiv)
          {
            // errs() << "This is Unsigned Division"
            //        << "\n";
          }
          if (inst.getOpcode() == Instruction::SDiv)
          {
            // errs() << "This is Signed Division"
            //        << "\n";
          }
          if (inst.getOpcode() == Instruction::Mul)
          {
            // errs() << "This is Multiplication"
            //        << "\n";
          }

          // see other classes, Instruction::Sub, Instruction::UDiv, Instruction::SDiv
          // errs() << "Operand(0)" << (*inst.getOperand(0))<<"\n";
          string resultString = "";
          auto *ptr = dyn_cast<User>(&inst);
          // errs() << "\t" << *ptr << "\n";
          int i = 0;
          for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it)
          {
            Value *v = dyn_cast<Value>(it);
            stringstream ss;
            ss << v;
            string value = ss.str();
            auto valueKey = hashTable.find(value);
            int result = 0;

            if (valueKey == hashTable.end())
            {
              count++;
              result = count;
              hashTable.insert(make_pair(value, count));
            }
            else
            {
              result = valueKey->second;
            }
            if (i == 0)
            {
              resultString += to_string(result) + " " + inst.getOpcodeName() + " ";
            }
            else
            {
              resultString += to_string(result);
            }
            i++;
            // errs() << "\t" << *(*it) << "\n";
            // errs() << dyn_cast<Value>(it) << "\n";
            // if ((*it)->hasName())
            // errs() << (*it)->getName() << "\n";
          }

          auto resultStringKey = hashTable.find(resultString);
          int resultStringNum;
          if (resultStringKey == hashTable.end())
          {
            count++;
            resultStringNum = count;
            hashTable.insert(make_pair(resultString, resultStringNum));
          }
          else
          {
            resultStringNum = resultStringKey->second;
          }

          Value *v = &inst;
          stringstream ss;
          ss << v;
          string value = ss.str();
          auto valueKey = hashTable.find(value);

          if (valueKey != hashTable.end())
          {
            hashTable.erase(valueKey);
          }

          hashTable.insert(make_pair(value, resultStringNum));

          // result print
          errs() << inst << "\t" << resultStringNum << " = " << resultString << "\n";
          // errs() << value << ":: " << resultStringNum << "\n";

          // auto *ptr2 = dyn_cast<User>(&inst);
          // for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it)
          // {
          //   llvm::User *instr = dyn_cast<User>(it);
          //   errs() << "custom"
          //          << "\n";

          //   for (int i = 0; i < instr->getNumOperands(); i++)
          //   {
          //     errs() << i << ": " << instr->getOperand(i)->getName() << ",";
          //   }
          //   errs() << "\n";
          // }
        } // end if
      }   // end for inst
      printHashTable(hashTable);
    } // end for block
  }

  // New PM implementation
  struct ValueNumberingPass : public PassInfoMixin<ValueNumberingPass>
  {

    // The first argument of the run() function defines on what level
    // of granularity your pass will run (e.g. Module, Function).
    // The second argument is the corresponding AnalysisManager
    // (e.g ModuleAnalysisManager, FunctionAnalysisManager)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      visitor(F);
      return PreservedAnalyses::all();
    }

    static bool isRequired() { return true; }
  };
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo()
{
  return {
      LLVM_PLUGIN_API_VERSION, "ValueNumberingPass", LLVM_VERSION_STRING,
      [](PassBuilder &PB)
      {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>)
            {
              if (Name == "value-numbering")
              {
                FPM.addPass(ValueNumberingPass());
                return true;
              }
              return false;
            });
      }};
}
