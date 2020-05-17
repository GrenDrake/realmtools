    std::ofstream realmDot("realms.dot");
    realmDot << "graph Realms {\n";
	realmDot << "\tsplines=\"true\";\n";
	realmDot << "\toverlap=\"false\";\n";
    for (const Realm *r : world.realms) {
        // realmDot << "\tident_" << r->ident << " [label=\"" << r->name << "\\n";
        // realmDot << r->ident << "\"];\n";
        realmDot << "\tident_" << r->ident << " [label=\"";
        realmDot << r->ident << " / " << r->group << "\"];\n";
        for (const Link &l : r->links) {
            if (l.linkTo < r->ident) continue;
            realmDot << "\tident_" << r->ident << " -- ident_" << l.linkTo << ";\n";
        }
    }
    realmDot << "}\n";
    realmDot.close();