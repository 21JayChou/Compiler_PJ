#include "TypeCheck.h"
#include<unordered_set>

//global tabels
typeMap func2retType; // function name to return type

// global token ids to type
typeMap g_token2Type;
// local token ids to type 
typeMap l_token2Type;


// local token ids to type, since func param can override global param
typeMap funcparam_token2Type;
vector<typeMap*> local_token2Type;


paramMemberMap func2Param;
paramMemberMap struct2Members;


// private util functions
void error_print(std::ostream& out, A_pos p, string info)
{
    out << "Typecheck error in line " << p->line << ", col " << p->col << ": " << info << std::endl;
    exit(0);
}

void error_kind(std::ostream& out, A_pos p, int kind){
    switch (kind)
    {
    case 1:
        error_print(out,p,"Variable conflict with function param!");
        break;
    
    case 2:
        error_print(out,p,"Variable conflict with local variable!");
        break;
    case 3:
        error_print(out,p,"Variable conflict with global variable!");
        break;
    }
}

int find_v(string &name){
    if(funcparam_token2Type.count(name))
        return 1;
    if(l_token2Type.count(name))
        return 2;
    if(g_token2Type.count(name))
        return 3;
    return 0;
}
tc_type get_token_type(string &name){
    int kind = find_v(name);
    if(kind == 1) return funcparam_token2Type[name];
    if(kind == 2) return l_token2Type[name];
    if(kind == 3) return g_token2Type[name];
    return nullptr;
}


// int find_array(string &name){
//  if(funcparam_array2Type.count(name))
//         return 1;
//     if(l_array2Type.count(name))
//         return 2;
//     if(g_array2Type.count(name))
//         return 3;
//     return 0;
// }

// void check_array(string &name,std::ostream& out, A_pos p){
//     int kind = find_array(name);
//     if(kind != 0){
//         error_kind(out,p,kind);
//     }
// }

tc_type_  check_type(aA_varDeclStmt vd){
    string id;
    if (vd->kind == A_varDeclStmtType::A_varDeclKind) {
        aA_varDecl vDecl = vd->u.varDecl;
        if (vDecl->kind == A_varDeclType::A_varDeclScalarKind) {
            id = *(vDecl->u.declScalar->id);
            return {vDecl->u.declScalar->type,0};
        } else if (vDecl->kind == A_varDeclType::A_varDeclArrayKind) {
            id = *(vDecl->u.declArray->id);
            return {vDecl->u.declArray->type,1};
        }
    } else {
        aA_varDef vDef = vd->u.varDef;
        if (vDef->kind == A_varDefType::A_varDefScalarKind) {
            id = *(vDef->u.defScalar->id);
            return {vDef->u.defScalar->type,0};
        } else if (vDef->kind == A_varDefType::A_varDefArrayKind) {
            id = *(vDef->u.defArray->id);
            return {vDef->u.defArray->type,1};
        }
    }
    return {};   
}

void add_v(string &name, tc_type tt, int kind){
    if(kind == 1){
        funcparam_token2Type[name] = tt;
    }
    else if(kind == 2){
        l_token2Type[name] = tt;
    }
    else{
        g_token2Type[name] = tt;
    }
}

void print_token_map(typeMap* map){
    for(auto it = map->begin(); it != map->end(); it++){
        std::cout << it->first << " : ";
        switch (it->second->type->type)
        {
        case A_dataType::A_nativeTypeKind:
            switch (it->second->type->u.nativeType)
            {
            case A_nativeType::A_intTypeKind:
                std::cout << "int";
                break;
            default:
                break;
            }
            break;
        case A_dataType::A_structTypeKind:
            std::cout << *(it->second->type->u.structType);
            break;
        default:
            break;
        }
        switch(it->second->isVarArrFunc){
            case 0:
                std::cout << " scalar";
                break;
            case 1:
                std::cout << " array";
                break;
            case 2:
                std::cout << " function";
                break;
        }
        std::cout << std::endl;
    }
}


void print_token_maps(){
    std::cout << "global token2Type:" << std::endl;
    print_token_map(&g_token2Type);
    std::cout << "local token2Type:" << std::endl;
    print_token_map(&funcparam_token2Type);
}


bool comp_aA_type(aA_type target, aA_type t){
    if(!target || !t)
        return false;
    if(target->type != t->type)
        return false;
    if(target->type == A_dataType::A_nativeTypeKind){
        if(target->u.nativeType != t->u.nativeType){
            std::cout<<"nativeType error!\n";
            return false;
        }
    }
    if(target->type == A_dataType::A_structTypeKind){
        if(target->u.structType != t->u.structType){
            std::cout<<"structType error!\n";
            return false;
        }
    }
    return true;
}


bool comp_tc_type(tc_type target, tc_type t){
    if(!target || !t)
        return false;
    
    // arr kind first
    //if (target->isVarArrFunc & t->isVarArrFunc == 0)
    if (target->isVarArrFunc != t->isVarArrFunc ){
        std::cout<<"isVarArrFunc error"<<"\n";
        return false;
    }
    
    // if target type is nullptr, alwayse ok
    return comp_aA_type(target->type, t->type);
}


tc_type tc_Type(aA_type t, uint isVarArrFunc){
    tc_type ret = new tc_type_;
    ret->type = t;
    ret->isVarArrFunc = isVarArrFunc;
    return ret;
}


tc_type tc_Type(aA_varDecl vd){
    if(vd->kind == A_varDeclType::A_varDeclScalarKind)
        return tc_Type(vd->u.declScalar->type, 0);
    else if(vd->kind == A_varDeclType::A_varDeclArrayKind)
        return tc_Type(vd->u.declArray->type, 1);
    return nullptr;
}


// public functions
void check_Prog(std::ostream& out, aA_program p)
{
    for (auto ele : p->programElements)
    {
        if(ele->kind == A_programVarDeclStmtKind){
            check_VarDecl(out, ele->u.varDeclStmt);
        }else if (ele->kind == A_programStructDefKind){
            check_StructDef(out, ele->u.structDef);
        }
    }
    
    for (auto ele : p->programElements){
        if(ele->kind == A_programFnDeclStmtKind){
            check_FnDeclStmt(out, ele->u.fnDeclStmt);
        }
        else if (ele->kind == A_programFnDefKind){
            check_FnDecl(out, ele->u.fnDef->fnDecl);
        }
    }

    for (auto ele : p->programElements){
        if(ele->kind == A_programFnDefKind){
            check_FnDef(out, ele->u.fnDef);
        }
        else if (ele->kind == A_programNullStmtKind){
            // do nothing
        }
    }

    out << "Typecheck passed!" << std::endl;
    return;
}


void check_VarDecl(std::ostream& out, aA_varDeclStmt vd,int scope)
{
    if (!vd)
        return;
    string name;
    if (vd->kind == A_varDeclStmtType::A_varDeclKind){
        // decl only
        aA_varDecl vdecl = vd->u.varDecl;
        if(vdecl->kind == A_varDeclType::A_varDeclScalarKind){
            name = *vdecl->u.declScalar->id;
            /* fill code here*/
            // if(vdecl->u.declScalar->type == nullptr)
            //     error_print(out,vdecl->u.declScalar->pos,"Not implemented!");
            int kind = find_v(name);
            if(kind != 0)error_kind(out,vdecl->u.declScalar->pos,kind);
            if(vdecl->u.declScalar->type->type == A_dataType::A_structTypeKind&&
            !struct2Members.count(*(vdecl->u.declScalar->type->u.structType)))
                error_print(out,vdecl->u.declScalar->pos,"Struct type is not defined!");
            add_v(name,tc_Type(vdecl),scope);

        }else if (vdecl->kind == A_varDeclType::A_varDeclArrayKind){
            name = *vdecl->u.declArray->id;
            /* fill code here*/
            if(vdecl->u.declArray->type == nullptr)
                error_print(out,vdecl->u.declArray->pos,"Not implemented!");
            int kind = find_v(name);
            if(kind != 0)error_kind(out,vdecl->u.declArray->pos,kind);
            if(vdecl->u.declArray->type->type == A_dataType::A_structTypeKind&&
            !struct2Members.count(*(vdecl->u.declArray->type->u.structType)))
                error_print(out,vdecl->u.declArray->pos,"Struct type is not defined!");
            add_v(name,tc_Type(vdecl),scope);
            }
    }
    else if (vd->kind == A_varDeclStmtType::A_varDefKind){
        // decl and def
        aA_varDef vdef = vd->u.varDef;
        if (vdef->kind == A_varDefType::A_varDefScalarKind){
            name = *vdef->u.defScalar->id;
            /* fill code here, allow omited type */
            int kind = find_v(name);
            if(kind != 0)error_kind(out,vdef->u.defScalar->pos,kind);
            tc_type tt = new tc_type_;
            if(vdef->u.defScalar->val->kind == A_rightValType::A_boolExprValKind){
                check_BoolExpr(out,vdef->u.defScalar->val->u.boolExpr);
                tt->isVarArrFunc=0;
                tt->type->pos = vdef->u.defScalar->val->u.boolExpr->pos;
                tt->type->type = A_dataType::A_nativeTypeKind;
                tt->type->u.nativeType = A_nativeType::A_intTypeKind;
            }
            else{
                tt = check_ArithExpr(out,vdef->u.defScalar->val->u.arithExpr);
            }
            if(vdef->u.defScalar->type == nullptr){
                add_v(name,tc_Type(tt->type,0),scope);
            }
            else{
                if(vdef->u.defScalar->type->type == A_dataType::A_structTypeKind&&
                !struct2Members.count(*(vdef->u.defScalar->type->u.structType)))
                error_print(out,vdef->u.defScalar->pos,"Struct type is not defined!");
                tc_type target = tc_Type(vdef->u.defScalar->type,0);
                if(!comp_tc_type(target,tt)){
                    error_print(out,vdef->u.defScalar->pos,"Type Error!");
                }
                add_v(name,target,scope);
            }

        }else if (vdef->kind == A_varDefType::A_varDefArrayKind){
            name = *vdef->u.defArray->id;
            /* fill code here, allow omited type */
            int kind = find_v(name);
            if(kind != 0)error_kind(out,vdef->u.defArray->pos,kind);
            if(vdef->u.defArray->len != vdef->u.defArray->vals.size())
                error_print(out,vdef->u.defArray->pos,"Length Error!");

            if(vdef->u.defArray->type!=nullptr&&
            vdef->u.defArray->type->type == A_dataType::A_structTypeKind&&
            !struct2Members.count(*(vdef->u.defArray->type->u.structType))){
                error_print(out,vdef->u.defScalar->pos,"Struct type is not defined!");
            }

            tc_type pre = nullptr;
            tc_type tt = nullptr;
            for(int i=0;i<vdef->u.defArray->vals.size();i++){
                auto rightVal = vdef->u.defArray->vals[i];
            if(rightVal->kind == A_rightValType::A_boolExprValKind){
                check_BoolExpr(out,rightVal->u.boolExpr);
                tt = new tc_type_;
                tt->isVarArrFunc=0;
                tt->type->pos = vdef->u.defScalar->val->u.boolExpr->pos;
                tt->type->type = A_dataType::A_nativeTypeKind;
                tt->type->u.nativeType = A_nativeType::A_intTypeKind;
            }
            else{
                tt = check_ArithExpr(out,vdef->u.defScalar->val->u.arithExpr);
            }
            if(vdef->u.defArray->type == nullptr){
                if(pre!=nullptr&&!comp_tc_type(pre,tt))
                    error_print(out,vdef->u.defArray->pos,"array elements type error!");
            }
            else{
                if(!comp_aA_type(vdef->u.defArray->type,tt->type)){
                    error_print(out,vdef->u.defArray->pos,"Type Error!");
                }
            }
            delete pre;
            pre = tt;
            }

            if(vdef->u.defArray->type == nullptr){
                add_v(name,tc_Type(tt->type,1),scope);
            }
            else{
                tc_type target = tc_Type(vdef->u.defArray->type,1);
                add_v(name,target,scope);
            }
        }
    }
    return;
}


void check_StructDef(std::ostream& out, aA_structDef sd)
{
    if (!sd)
        return;
    string name = *sd->id;
    if (struct2Members.find(name) != struct2Members.end())
        error_print(out, sd->pos, "This id is already defined!");
    struct2Members[name] = &(sd->varDecls);
    return;
}


void check_FnDecl(std::ostream& out, aA_fnDecl fd)
{
    if (!fd)
        return;
    string name = *fd->id;

    // if already declared, should match
    if (func2Param.find(name) != func2Param.end()){
        // is function ret val matches
        /* fill code here */
        if(!comp_aA_type(fd->type,func2retType[name]->type)){
        error_print(out,fd->pos,"Ret val is not matched!");
        }
        // is function params matches decl
        /* fill code here */
        auto requiredParams = *func2Param[name];
        auto curParams = fd->paramDecl->varDecls;
        if(requiredParams.size()!=curParams.size())
            error_print(out,fd->pos,"Number of params is not matched!");
        int n = requiredParams.size();
        for(int i=0;i<n;i++){
            auto p1 = requiredParams[i];
            auto p2 = curParams[i];
            if(p1->kind!=p2->kind)
                error_print(out,fd->pos,"Params not matched!");
            switch(p1->kind){
                case A_varDeclType::A_varDeclScalarKind:{
                    aA_varDeclScalar vds1 = p1->u.declScalar,vds2 = p2->u.declScalar;
                    if(*(vds1->id)!=*(vds2->id)||!comp_aA_type(vds1->type,vds2->type))
                        error_print(out,vds2->pos,"Params not matched!");
        /* fill code here */
                    if(vds2->type->type == A_dataType::A_structTypeKind&&!struct2Members.count(*(vds2->type->u.structType)))
                        error_print(out,vds2->pos,"Struct is not defined!");
                    break;
                }
                case A_varDeclType::A_varDeclArrayKind:{
                    aA_varDeclArray vda1 = p1->u.declArray,vda2 = p2->u.declArray;
                    if(*(vda1->id)!=*(vda2->id)||vda1->len!=vda2->len||!comp_aA_type(vda1->type,vda2->type))
                        error_print(out,vda2->pos,"Params not matched!");
                     if(vda2->type->type == A_dataType::A_structTypeKind&&!struct2Members.count(*(vda2->type->u.structType)))
                        error_print(out,vda2->pos,"Struct is not defined!");
                    break;
                }

            }
        }
    }else{
        // if not defined
        /* fill code here */
        vector<aA_varDecl>* params = new vector<aA_varDecl>();
        auto curParams = fd->paramDecl->varDecls;
        int n = curParams.size();
        for(int i=0;i<n;i++){
            auto p = curParams[i];
            switch(p->kind){
                case A_varDeclType::A_varDeclScalarKind:{
                    aA_varDeclScalar vds = p->u.declScalar;
                    if(vds->type->type == A_dataType::A_structTypeKind&&!struct2Members.count(*(vds->type->u.structType)))
                        error_print(out,vds->pos,"Struct is not defined!");
                    break;
                }
                case A_varDeclType::A_varDeclArrayKind:{
                    aA_varDeclArray vda = p->u.declArray;
                    if(vda->type->type == A_dataType::A_structTypeKind&&!struct2Members.count(*(vda->type->u.structType)))
                        error_print(out,vda->pos,"Struct is not defined!");
                }
            }
            (*params).push_back(p);
        }
        func2Param[name] = params;
        if(fd->type->type == A_dataType::A_structTypeKind&&!struct2Members.count(*(fd->type->u.structType))){
            error_print(out,fd->type->pos,"Struct is not defined!");
        }
        func2retType[name] = tc_Type(fd->type,0);
    }
    return;
}


void check_FnDeclStmt(std::ostream& out, aA_fnDeclStmt fd)
{
    if (!fd)
        return;
    check_FnDecl(out, fd->fnDecl);
    return;
}


void check_FnDef(std::ostream& out, aA_fnDef fd)
{
    if (!fd)
        return;
    // should match if declared
    check_FnDecl(out, fd->fnDecl);
    string name = *(fd->fnDecl->id);
    // add params to local tokenmap, func params override global ones
    funcparam_token2Type.clear();
    l_token2Type.clear();
    for (aA_varDecl vd : fd->fnDecl->paramDecl->varDecls)
    {
        /* fill code here */
        switch (vd->kind)
        {
        case A_varDeclType::A_varDeclScalarKind :{
            funcparam_token2Type[*(vd->u.declScalar->id)] = tc_Type(vd->u.declScalar->type,0);
            break;
        }
        case A_varDeclType::A_varDeclArrayKind :{
            funcparam_token2Type[*(vd->u.declArray->id)] = tc_Type(vd->u.declArray->type,1);
            break;
        }
        }
    }

    /* fill code here */
    for (aA_codeBlockStmt stmt : fd->stmts)
    {
        check_CodeblockStmt(out, stmt);
        // return value type should match
        /* fill code here */
        if(stmt->kind == A_codeBlockStmtType::A_returnStmtKind){
            auto rightVal = stmt->u.returnStmt->retVal;
            tc_type tt = new tc_type_;
            if(rightVal->kind == A_rightValType::A_boolExprValKind){
                check_BoolExpr(out,rightVal->u.boolExpr);
                tt->isVarArrFunc=0;
                tt->type->pos = rightVal->u.boolExpr->pos;
                tt->type->type = A_dataType::A_nativeTypeKind;
                tt->type->u.nativeType = A_nativeType::A_intTypeKind;
            }
            else{
                tt = check_ArithExpr(out,rightVal->u.arithExpr);
            }
            if(!comp_tc_type(tt,func2retType[name]))
                error_print(out,rightVal->pos,"Ret val error!");
        }        
    }

    return;
}


void check_CodeblockStmt(std::ostream& out, aA_codeBlockStmt cs){
    l_token2Type.clear();
    if(!cs)
        return;
    // variables declared in a code block should not duplicate with outer ones.
    switch (cs->kind)
    {
    case A_codeBlockStmtType::A_varDeclStmtKind:
        check_VarDecl(out, cs->u.varDeclStmt);
        break;
    case A_codeBlockStmtType::A_assignStmtKind:
        check_AssignStmt(out, cs->u.assignStmt);
        break;
    case A_codeBlockStmtType::A_ifStmtKind:
        check_IfStmt(out, cs->u.ifStmt);
        break;
    case A_codeBlockStmtType::A_whileStmtKind:
        check_WhileStmt(out, cs->u.whileStmt);
        break;
    case A_codeBlockStmtType::A_callStmtKind:
        check_CallStmt(out, cs->u.callStmt);
        break;
    case A_codeBlockStmtType::A_returnStmtKind:
        check_ReturnStmt(out, cs->u.returnStmt);
        break;
    default:
        break;
    }
    return;
}


void check_AssignStmt(std::ostream& out, aA_assignStmt as){
    if(!as)
        return;
    string name;
    tc_type deduced_type = nullptr; // deduced type if type is omitted at decl
    tc_type t_l = nullptr,t_r = nullptr;
    if(as->rightVal->kind == A_rightValType::A_boolExprValKind){
        check_BoolExpr(out,as->rightVal->u.boolExpr);
        t_r = new tc_type_;
        t_r->isVarArrFunc=0;
        t_r->type->type = A_dataType::A_nativeTypeKind;
        t_r->type->u.nativeType = A_nativeType::A_intTypeKind;
    }
    else{
        t_r = check_ArithExpr(out,as->rightVal->u.arithExpr);
    }
    switch (as->leftVal->kind)
    {
        case A_leftValType::A_varValKind:{
            name = *as->leftVal->u.id;
            /* fill code here */
            if(func2Param.count(name))
                error_print(out,as->leftVal->pos,"cannot assign a value to function!");
            int kind = find_v(name);
            if(kind==0){
                error_print(out,as->leftVal->pos,"Undefined var!");
            }
            else{
                if(kind==1){
                    t_l = funcparam_token2Type[name];
                    if(t_l->type == nullptr){
                        funcparam_token2Type[name]=tc_Type(t_r->type,0);
                        return;
                    }
                }
                else if(kind==2){
                    t_l = l_token2Type[name];
                    if(t_l->type == nullptr){
                        l_token2Type[name]=tc_Type(t_r->type,0);
                        return;
                    }
                }
                else{
                     t_l = g_token2Type[name];
                    if(t_l->type == nullptr){
                        g_token2Type[name]=tc_Type(t_r->type,0);
                        return;
                    }
                }
                if(t_l->isVarArrFunc==1)
                    error_print(out,as->leftVal->pos,"Assigning a value to an array is not allowed!");
            }
            break;
        }
            break;
        case A_leftValType::A_arrValKind:{
            name = *as->leftVal->u.arrExpr->arr->u.id;
            /* fill code here */
            check_ArrayExpr(out, as->leftVal->u.arrExpr);
        }
            break;
        case A_leftValType::A_memberValKind:{
            /* fill code here */
            aA_memberExpr me = as->leftVal->u.memberExpr;
            t_l = check_MemberExpr(out, me);
        }
            break;
    }
    if(!comp_tc_type(t_l,t_r)){
        error_print(out, as->pos, "Assignment with different types is not allowed!");
    }

    return;
}


void check_ArrayExpr(std::ostream& out, aA_arrayExpr ae){
    if(!ae)
        return;
    string name = *ae->arr->u.id;
    // check array name
    /* fill code here */
    tc_type t_l;
    if(find_v(name)==0){
        error_print(out,ae->arr->pos,"Undefined array name!");
    }
    t_l = get_token_type(name);
    if(t_l->isVarArrFunc!=1){
        error_print(out,ae->arr->pos,"This is not an array!");
    }    
    // check index
    /* fill code here */
        if (ae->idx->kind == A_indexExprKind::A_numIndexKind) {
        // if idx is num, must >= 0
        if (ae->idx->u.num < 0)
            error_print(out, ae->pos, "Array index must be a positive int!");
    } else if (ae->idx->kind == A_indexExprKind::A_idIndexKind) {
        // if idx is var, must be a int
        string idx_name = *(ae->idx->u.id);
        if (find_v(idx_name) == 0)
            error_print(out, ae->pos, "Array index id is not defined!");
        tc_type idx_type = get_token_type(idx_name);
        if (idx_type->type->type != A_dataType::A_nativeTypeKind || idx_type->type->u.nativeType != A_nativeType::A_intTypeKind)
            error_print(out, ae->pos, "Array index is not int!");
    }
    return;
}

 
tc_type check_MemberExpr(std::ostream& out, aA_memberExpr me){
    // check if the member exists and return the tyep of the member
    if(!me)
        return nullptr;
    string name = *me->structId->u.id;
    // check struct name
    /* fill code here */
    if(find_v(name)==0)
        error_print(out,me->structId->pos,"Undefined var!");
    // if(struct2Members.count(me->structId)==0){
    //     error_print(out,me->structId->pos,"Struct has not defined!");
    // }
    tc_type tt = get_token_type(name);
     if (tt->type->type != A_dataType::A_structTypeKind)
        error_print(out, me->pos, "Variable is not a struct!");
    string struct_name = *tt->type->u.structType;
    // check member name
    /* fill code here */
    string member_id = *(me->memberId);
    for(auto varDecl:*(struct2Members[struct_name])){
        switch (varDecl->kind) {
        case A_varDeclType::A_varDeclScalarKind:
            if (*(varDecl->u.declScalar->id) == member_id) {
                return tc_Type(varDecl->u.declScalar->type,0);
            }
            break;
        case A_varDeclType::A_varDeclArrayKind:
            if (*(varDecl->u.declArray->id) == member_id) {
                return tc_Type(varDecl->u.declArray->type,1);
            }
            break;
        }
    }
    error_print(out,me->pos,"Member does not exist in this struct type!");   
    return nullptr;
}


void check_IfStmt(std::ostream& out, aA_ifStmt is){
    if(!is)
        return;
    check_BoolExpr(out, is->boolExpr);
    /* fill code here, take care of variable scope */
    std::unordered_set<string> tokens;
    for(aA_codeBlockStmt s : is->ifStmts){
        check_CodeblockStmt(out, s);
        if (s->kind == A_codeBlockStmtType::A_varDeclStmtKind) {
            // add var to local var map
            check_VarDecl(out,s->u.varDeclStmt,2);
            string name;
            if(s->u.varDeclStmt->kind == A_varDeclStmtType::A_varDeclKind){
                aA_varDecl vd = s->u.varDeclStmt->u.varDecl;
                if(vd->kind == A_varDeclType::A_varDeclScalarKind)
                    name = *vd->u.declScalar->id;
                else name = *vd->u.declArray->id;
            }
            else{
                aA_varDef vd = s->u.varDeclStmt->u.varDef;
                if(vd->kind == A_varDefType::A_varDefScalarKind)
                    name = *vd->u.defScalar->id;
                else name = *vd->u.defArray->id;
            }
            tokens.insert(name);
        }
    }
     for (string token : tokens)
        l_token2Type.erase(token);
    tokens.clear();
    /* fill code here */    
    for(aA_codeBlockStmt s : is->elseStmts){
        check_CodeblockStmt(out, s);
        if (s->kind == A_codeBlockStmtType::A_varDeclStmtKind) {
            // add var to local var map
            check_VarDecl(out,s->u.varDeclStmt,2);
            string name;
            if(s->u.varDeclStmt->kind == A_varDeclStmtType::A_varDeclKind){
                aA_varDecl vd = s->u.varDeclStmt->u.varDecl;
                if(vd->kind == A_varDeclType::A_varDeclScalarKind)
                    name = *vd->u.declScalar->id;
                else name = *vd->u.declArray->id;
            }
            else{
                aA_varDef vd = s->u.varDeclStmt->u.varDef;
                if(vd->kind == A_varDefType::A_varDefScalarKind)
                    name = *vd->u.defScalar->id;
                else name = *vd->u.defArray->id;
            }
            tokens.insert(name);
        }
    }
    /* fill code here */
    for (string token : tokens)
        l_token2Type.erase(token);
    return;
}


void check_BoolExpr(std::ostream& out, aA_boolExpr be){
    if(!be)
        return;
    switch (be->kind)
    {
    case A_boolExprType::A_boolBiOpExprKind:
        check_BoolExpr(out, be->u.boolBiOpExpr->left);
        check_BoolExpr(out, be->u.boolBiOpExpr->right);
        break;
    case A_boolExprType::A_boolUnitKind:
        check_BoolUnit(out, be->u.boolUnit);
        break;
    default:
        break;
    }
    return;
}


void check_BoolUnit(std::ostream& out, aA_boolUnit bu){
    if(!bu)
        return;
    switch (bu->kind)
    {
        case A_boolUnitType::A_comOpExprKind:{
            /* fill code here */
            tc_type tl = check_ExprUnit(out,bu->u.comExpr->left);
            tc_type tr = check_ExprUnit(out,bu->u.comExpr->right);
            if(!comp_tc_type(tl,tr))
                error_print(out,bu->u.comExpr->pos,"These two expression unit are not comparable!");
        }
            break;
        case A_boolUnitType::A_boolExprKind:
            check_BoolExpr(out, bu->u.boolExpr);
            break;
        case A_boolUnitType::A_boolUOpExprKind:
            check_BoolUnit(out, bu->u.boolUOpExpr->cond);
            break;
        default:
            break;
    }
    return;
}


tc_type check_ExprUnit(std::ostream& out, aA_exprUnit eu){
    // return the aA_type of expr eu
    if(!eu)
        return nullptr;
    tc_type ret;
    switch (eu->kind)
    {
        case A_exprUnitType::A_idExprKind:{
            /* fill code here */
            string id = *(eu->u.id);
            if (find_v(id) == 0)
            error_print(out, eu->pos, "Variable is not defined!");
            ret = get_token_type(id);
        }
            break;
        case A_exprUnitType::A_numExprKind:{
            aA_type numt = new aA_type_;
            numt->pos = eu->pos;
            numt->type = A_dataType::A_nativeTypeKind;
            numt->u.nativeType = A_nativeType::A_intTypeKind;
            ret = tc_Type(numt, 0);
        }
            break;
        case A_exprUnitType::A_fnCallKind:{
            check_FuncCall(out, eu->u.callExpr);
            // check_FuncCall will check if the function is defined
            /* fill code here */
            ret = func2retType[*(eu->u.callExpr->fn)];
        }
            break;
        case A_exprUnitType::A_arrayExprKind:{
            check_ArrayExpr(out, eu->u.arrayExpr);
            /* fill code here */
            ret = get_token_type(*eu->u.arrayExpr->arr->u.id);
        }
            break;
        case A_exprUnitType::A_memberExprKind:{
            ret = check_MemberExpr(out, eu->u.memberExpr);
        }
            break;
        case A_exprUnitType::A_arithExprKind:{
            ret = check_ArithExpr(out, eu->u.arithExpr);
        }
            break;
        case A_exprUnitType::A_arithUExprKind:{
            ret = check_ExprUnit(out, eu->u.arithUExpr->expr);
        }
            break;
    }
    return ret;
}


tc_type check_ArithExpr(std::ostream& out, aA_arithExpr ae){
    if(!ae)
        return nullptr;
    tc_type ret;
    switch (ae->kind)
    {
        case A_arithExprType::A_arithBiOpExprKind:{
            ret = check_ArithExpr(out, ae->u.arithBiOpExpr->left);
            tc_type rightTyep = check_ArithExpr(out, ae->u.arithBiOpExpr->right);
            if(ret->type->type > 0 || ret->type->type != A_dataType::A_nativeTypeKind || ret->type->u.nativeType != A_nativeType::A_intTypeKind ||
            rightTyep->type->type > 0 || rightTyep->type->type != A_dataType::A_nativeTypeKind || rightTyep->type->u.nativeType != A_nativeType::A_intTypeKind)
                error_print(out, ae->pos, "Only int can be arithmetic expression operation values!");
        }
            break;
        case A_arithExprType::A_exprUnitKind:
            ret = check_ExprUnit(out, ae->u.exprUnit);
            break;
    }
    return ret;
}


void check_FuncCall(std::ostream& out, aA_fnCall fc){
    if(!fc)
        return;
    // check if function defined
    string func_name = *fc->fn;
    /* fill code here */
    if(func2Param.count(func_name)==0)
        error_print(out,fc->pos,"Cannot find the specified function!");
    // check if parameter list matches
    auto req_param = func2Param[func_name];
    auto vals = &(fc->vals);
    if (vals->size() != req_param->size())
        error_print(out, fc->pos, "Number of parameters does not match!");
    for(int i = 0; i < fc->vals.size(); i++){
        /* fill code here */
        switch ((*req_param)[i]->kind) {
        case A_varDeclType::A_varDeclScalarKind: {
            tc_type t = check_ArithExpr(out, (*vals)[i]->u.arithExpr);
            tc_type t_req = tc_Type((*req_param)[i]->u.declScalar->type,0);
            if (!comp_tc_type(t,t_req))
                error_print(out, (*vals)[i]->pos, "The type of input parameter does not meet the requirements");
        } break;
        case A_varDeclType::A_varDeclArrayKind: {


            if ((*vals)[i]->u.arithExpr->kind != A_arithExprType::A_exprUnitKind)
                error_print(out, (*vals)[i]->pos, "The type of input parameter does not meet the requirements");
            aA_exprUnit eu = (*vals)[i]->u.arithExpr->u.exprUnit;
            if (eu->kind != A_exprUnitType::A_idExprKind)
                error_print(out, (*vals)[i]->pos, "The type of input parameter does not meet the requirements");
            string id = *(eu->u.id);
            if (find_v(id) == 0)
                error_print(out, (*vals)[i]->pos, "Array is not defined or this id is not an array!");
            tc_type t = get_token_type(id);
            tc_type t_req= tc_Type((*req_param)[i]->u.declArray->type,1);
            if (!comp_tc_type(t,t_req))
                error_print(out, (*vals)[i]->pos, "The type of input parameter does not meet the requirements");
        } break;
        }
    }
    return ;
}


void check_WhileStmt(std::ostream& out, aA_whileStmt ws){
    if(!ws)
        return;
    check_BoolExpr(out, ws->boolExpr);
    /* fill code here, take care of variable scope */
    std::unordered_set<string> tokens;
    for(aA_codeBlockStmt s : ws->whileStmts){
        check_CodeblockStmt(out, s);
        if (s->kind == A_codeBlockStmtType::A_varDeclStmtKind) {
            // add var to local var map
            check_VarDecl(out,s->u.varDeclStmt,2);
            string name;
            if(s->u.varDeclStmt->kind == A_varDeclStmtType::A_varDeclKind){
                aA_varDecl vd = s->u.varDeclStmt->u.varDecl;
                if(vd->kind == A_varDeclType::A_varDeclScalarKind)
                    name = *vd->u.declScalar->id;
                else name = *vd->u.declArray->id;
            }
            else{
                aA_varDef vd = s->u.varDeclStmt->u.varDef;
                if(vd->kind == A_varDefType::A_varDefScalarKind)
                    name = *vd->u.defScalar->id;
                else name = *vd->u.defArray->id;
            }
            tokens.insert(name);

        }
    }
    /* fill code here */
     for (string token : tokens)
        l_token2Type.erase(token);    
    return;
}


void check_CallStmt(std::ostream& out, aA_callStmt cs){
    if(!cs)
        return;
    check_FuncCall(out, cs->fnCall);
    return;
}


void check_ReturnStmt(std::ostream& out, aA_returnStmt rs){
    if(!rs)
        return;
    return;
}

