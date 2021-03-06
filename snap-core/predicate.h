#ifndef PREDICATE_H
#define PREDICATE_H

typedef enum TAttrType_ {atInt, atFlt, atStr} TAttrType;   // attribute type
typedef enum {NOT, AND, OR, NOP} TPredOp; // boolean operators for selection predicates
typedef enum {LT = 0, LTE, EQ, NEQ, GTE, GT, SUBSTR, SUPERSTR} TPredComp; // comparison operators for selection predicates

class TPredicate {
public:
  class TAtomicPredicate;
protected:
  static const TAtomicPredicate NonAtom;
public:
  class TAtomicPredicate{
    TAttrType Type;
    TBool IsConst;
    TPredComp Compare;
    TStr Lvar;
    TStr Rvar;
    TInt IntConst;
    TFlt FltConst;
    TStr StrConst;
  public:
    TAtomicPredicate():Type(NonAtom.Type), IsConst(NonAtom.IsConst), Compare(NonAtom.Compare), Lvar(NonAtom.Lvar),
      Rvar(NonAtom.Rvar), IntConst(NonAtom.IntConst), FltConst(NonAtom.FltConst), StrConst(NonAtom.StrConst){}
    TAtomicPredicate(TAttrType Typ, TBool IsCnst, TPredComp Cmp, TStr L, TStr R, TInt ICnst, TFlt FCnst, TStr SCnst):
      Type(Typ), IsConst(IsCnst), Compare(Cmp), Lvar(L), Rvar(R), IntConst(ICnst), FltConst(FCnst), StrConst(SCnst){}
    TAtomicPredicate(TAttrType Typ, TBool IsCnst, TPredComp Cmp, TStr L, TStr R):
      Type(Typ), IsConst(IsCnst), Compare(Cmp), Lvar(L), Rvar(R), IntConst(0), FltConst(0), StrConst(""){}
    friend class TPredicate;
  };

  class TPredicateNode{
  //protected:
  public:
    TPredOp Op;
    TBool Result;
    TAtomicPredicate Atom;
    TPredicateNode* Parent;
    TPredicateNode* Left;
    TPredicateNode* Right;
  public:
    TPredicateNode(): Op(NOP), Result(false), Atom(), Parent(NULL), Left(NULL), Right(NULL){}
    // constructor for atomic predicate node (leaf)
    TPredicateNode(const TAtomicPredicate& A): Op(NOP), Result(false), Atom(A), Parent(NULL), Left(NULL), Right(NULL){}
    // constructor for logical operation predicate node (internal node)
    TPredicateNode(TPredOp Opr): Op(Opr), Result(false), Atom(), Parent(NULL), Left(NULL), Right(NULL){}
    TPredicateNode(const TPredicateNode& P): Op(P.Op), Result(P.Result), Atom(P.Atom), Parent(P.Parent), Left(P.Left), Right(P.Right){}
    void AddLeftChild(TPredicateNode* Child){ Left = Child; Child->Parent = this;}
    void AddRightChild(TPredicateNode* Child){ Right = Child; Child->Parent = this;}
    void GetVariables(TStrV& Variables);
    friend class TPredicate;
  };

protected:
  THash<TStr, TInt> IntVars;
  THash<TStr, TFlt> FltVars;
  THash<TStr, TStr> StrVars;
  TPredicateNode* Root;
public:
  TPredicate(): IntVars(), FltVars(), StrVars(){}
  TPredicate(TPredicateNode* R):IntVars(), FltVars(), StrVars(), Root(R){}
  TPredicate(const TPredicate& Pred): IntVars(Pred.IntVars), FltVars(Pred.FltVars), StrVars(Pred.StrVars), Root(Pred.Root){}
  void GetVariables(TStrV& Variables);
  // set variable value in the predicate or all the children that use it
  void SetIntVal(TStr VarName, TInt VarVal){ IntVars.AddDat(VarName, VarVal);}
  void SetFltVal(TStr VarName, TFlt VarVal){ FltVars.AddDat(VarName, VarVal);}
  void SetStrVal(TStr VarName, TStr VarVal){ StrVars.AddDat(VarName, VarVal);}
  TBool Eval();
  TBool EvalAtomicPredicate(const TAtomicPredicate& Atom);

  template <class T>
  static TBool EvalAtom(T Val1, T Val2, TPredComp Cmp){
    switch(Cmp){
      case LT: return Val1 < Val2;
      case LTE: return Val1 <= Val2;
      case EQ: return Val1 == Val2;
      case NEQ: return Val1 != Val2;
      case GTE: return Val1 >= Val2;
      case GT: return Val1 > Val2;
      default: return false;
    }
  };

  static TBool EvalStrAtom(TStr Val1, TStr Val2, TPredComp Cmp){
    switch(Cmp){
      case LT: return Val1 < Val2;
      case LTE: return Val1 <= Val2;
      case EQ: return Val1 == Val2;
      case NEQ: return Val1 != Val2;
      case GTE: return Val1 >= Val2;
      case GT: return Val1 > Val2;
      case SUBSTR: return Val2.IsStrIn(Val1);
      case SUPERSTR: return Val1.IsStrIn(Val2);
      default: return false;
    }
  }
};
#endif // PREDICATE_H

