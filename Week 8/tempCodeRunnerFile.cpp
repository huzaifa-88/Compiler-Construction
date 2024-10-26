void parseBlock(){
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF){
            parseStatement();
        }
        expect(T_RBRACE);
    }