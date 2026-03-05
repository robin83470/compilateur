#include "CodeTest.h"

antlrcpp::Any CodeTest::visitProg(ifccParser::ProgContext *ctx) 
{
    

    int n = ctx->expr().size();
    //std::cout << "# "<< n <<" children of type expr\n" ;
    for (int i = 0; i < n; i++) {
        visit(ctx->expr(i)); 
    }

    visit(ctx->return_stmt());
    for (const auto& [name, symbol] : mem) {
        if (symbol.used) {
            //std::cout << "used " << name << " at offset " << symbol.index << "\n";
        } else {
            warning_.push_back("ligne " + std::to_string(symbol.decline) + " var " + name + " declared but not used\n");
        }
    }


    for (const auto& err : erreur_) {
        std::cerr << err;
    }

    if (!warning_.empty()) {
        std::cerr << "Warning " << erreur_.size() << "\n";
    }

    if (!erreur_.empty()) {
        std::cerr << "Compilation failed with " << erreur_.size() << " error(s).\n";
        exit(1);
    }

    
    

    

    return 0;
}


antlrcpp::Any CodeTest::visitReturn_const_stmt(ifccParser::Return_const_stmtContext *ctx)
{


    std::cout << "# coucou1\n" ;


    

    return 0;
}



antlrcpp::Any CodeTest::visitReturn_var_stmt(ifccParser::Return_var_stmtContext *ctx)
{

    std::string name = ctx->VAR()->getText();

    if (mem.find(name) == mem.end()) {
        erreur_.push_back("ligne " + std::to_string(ctx->VAR()->getSymbol()->getLine()) + " var " + name + " not declared\n");
    } 
    else {
        mem.find(name)->second.used = true;
        //std::cout << "# sym " << name << " already -> " << mem[name].index << "\n";
    }
    

    

    return 0;
}



antlrcpp::Any CodeTest::visitAssignment_decla_const(
    ifccParser::Assignment_decla_constContext *ctx)
{
    std::string name = ctx->v1->getText();
    int c = stoi(ctx->c1->getText());

    if (mem.find(name) == mem.end()) {
        mem[name].index = nextOffset;
        mem[name].used = false;
        mem[name].decline = ctx->v1->getLine();
        nextOffset += 4;
        //std::cout << "# sym " << name << " -> " << mem[name].index << "\n";
    } else {
        erreur_.push_back("ligne " + std::to_string(mem[name].decline) + " deja declare\n");
    }
    return 0;
}


antlrcpp::Any CodeTest::visitAssignment_decla_var(ifccParser::Assignment_decla_varContext *ctx)
{
    std::string v1 = ctx->v1->getText();
    std::string v2 = ctx->v2->getText();

    if (mem.find(v2) == mem.end()) {
        erreur_.push_back("var pas déclarée " + v2 + " pas declaré"  + "\n");
        //std::cout << "var v2 pas decla" << v2 << " -> " << v2 << "\n";
    } 
    else {
        if (mem.find(v1) == mem.end()) {
            mem[v1].index = nextOffset;
            mem[v1].used = false;
            mem[v1].decline = ctx->v1->getLine();
            nextOffset += 4;
            mem.find(v2)->second.used = true;
            //std::cout << "# sym " << v1 << " -> " << mem[v1].index << "\n";
        }
        else {
            
             erreur_.push_back("ligne " + std::to_string(mem[v1].decline) + " deja declare\n");
        }
    }
    return 0;
}



antlrcpp::Any CodeTest::visitAssignment_vv(ifccParser::Assignment_vvContext *ctx)
{
    std::string v1 = ctx->v1->getText();
    std::string v2 = ctx->v2->getText();

    if (mem.find(v2) == mem.end()) {
        std::cout << "var v2 pas decla" << v2 << " -> " << v2 << "\n";
    } 
    else {
        if (mem.find(v1) == mem.end()) {
            
            std::cout << "var v1 pas decla" << v1 << " -> " << v1 << "\n";
        }
        else {
            mem.find(v1)->second.used = true;
            mem.find(v2)->second.used = true;
        }
    }
    return 0;
}

antlrcpp::Any CodeTest::visitAssignment_vc(ifccParser::Assignment_vcContext *ctx)
{
    std::string v1 = ctx->v1->getText();
    int c1 = stoi(ctx->c1->getText());

    if (mem.find(v1) == mem.end()) {
        std::cout << "var v1 pas decla" << v1 << " -> " << v1 << "\n";
    } 
    else {
        mem.find(v1)->second.used = true;
        //std::cout << "# sym " << v1 << " already -> " << mem[v1].index << "\n";
    }
    return 0;
}



antlrcpp::Any CodeTest::visitAssignment_decla(ifccParser::Assignment_declaContext *ctx)
{
    std::string name = ctx->VAR()->getText();

    if (mem.find(name) == mem.end()) {
        mem[name].index = nextOffset;
        mem[name].used = false;
        mem[name].decline = ctx->v1->getLine();
        nextOffset += 4;
        //std::cout << "# sym " << name << " -> " << mem[name].index << "\n";
    } else {
        erreur_.push_back("ligne " + std::to_string(mem[name].decline) + " deja declare\n");
    }
    return 0;
}
