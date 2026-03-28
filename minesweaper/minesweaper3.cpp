#include <iostream>
#include <vector>
#include <random>
#include <windows.h>


std::vector<int> dxs = {-1, -1, -1, 0, 0, 1, 1, 1};
std::vector<int> dys = {-1, 0, 1, -1, 1, -1, 0, 1};

class minesweaper{
        
    public:
        int size;
        int mine_num;
        int frag_num;
        const int mine;
        const int frag;
        const int cover;
        std::vector<std::vector<int>> display_board;
    private:
        double mine_ratio;
        std::vector<std::vector<bool>> mine_board;
        std::vector<std::vector<bool>> cover_board;
        std::vector<std::vector<bool>> frag_board;
        std::vector<std::vector<int>> num_board;

    public:
        minesweaper():
            size(20),//ゲームボードサイズ
            mine_ratio(0.15),//地雷の割合
            cover(10),//
            frag(11),
            mine(9),
            mine_board(size, std::vector(size,false)),
            cover_board(size, std::vector(size,true)),
            frag_board(size, std::vector(size,false)),
            num_board(size, std::vector(size,0)),
            display_board(size, std::vector(size,10))
        {
            mine_num = size*size*mine_ratio;
            frag_num = mine_num;
        }

        void mine_display(){//はじめに地雷の位置公開！！
            std::cout << frag_num << "/" <<  mine_num << std::endl;
            
            //列番号
            std::cout << "x\\y";
            for(int i = 0; i < size; i++){
                std::cout << (i < 10 ? "|" : "") << i << '|';
            }
            std::cout << std::endl;

            for(int i = 0; i < size; i++){
                //行番号
                std::cout << (i < 10 ? "  " : " ") << i;
                
                //メイン
                for(int j = 0; j < size; j++){
                    if(mine_board[i][j]){
                        std::cout << "M";
                    }
                    else{
                        std::cout << " - ";
                    }
                }
                std::cout << std::endl;
            }

        }


    
        bool hanni(int x, int y){//範囲確認
            return 0 <= x && x < size && 0 <= y && y < size;
        }

        
        //初期設定地雷の設置など
        void placemines(int x, int y){
            cover_board[x][y] = false;
            
            //いったん周りの9マスに地雷を設置
            mine_board[x][y] = true;
            for(int i = 0; i < 8; i++){
                int mx = x+dxs[i] ;
                int my = y+dys[i] ;
                if(hanni(mx, my)){
                mine_board[mx][my] = true;
                }
            }
             
            //初期化
            std::random_device rd;
            std::mt19937 gen(rd());
            //範囲指定
            std::uniform_int_distribution<> dist(0, size*size - 1);

            //地雷の設置
            int i = 0;
            while(i < mine_num){
                int random_num = dist(gen);
                int m = random_num / size;
                int n = random_num % size;

                if(mine_board[m][n]){
                    continue;
                }
                else{
                    mine_board[m][n] = true;
                    i++;
                }
            }
            
            //周りの地雷を消す
            mine_board[x][y] = false;
            for(int i = 0; i < 8; i++){
                int nx = x + dxs[i] ;
                int ny = y + dys[i] ;
                if(hanni(nx, ny)){
                mine_board[nx][ny] = false;
                }
            }

            //数字盤を生成
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    if(mine_board[i][j]){
                        for(int k = 0; k < 8; k++){
                            int X = i + dxs[k];
                            int Y = j + dys[k];
                            if(hanni(X, Y)){
                                num_board[X][Y]++;
                            }
                        }
                    }
                }
            }
            
            chainfunc(x, y);
        }




    private:
        //開ける
        void openfunc(int open_x, int open_y){
            if(hanni(open_x, open_y)){
                if(cover_board[open_x][open_y] && !frag_board[open_x][open_y]){//カバーがあるがフラグがない場合
                    cover_board[open_x][open_y] = false;
                    chainfunc(open_x, open_y);
                } 
                
                //あしすと
                int around_frag_num = 0;
                for(int i = 0; i < 8; i++){//周りのフラグの数を数える
                    int mx = open_x + dxs[i];
                    int my = open_y + dys[i];
                    if(hanni(mx, my) && frag_board[mx][my]){
                        around_frag_num++;
                    }
                }

                if(around_frag_num == num_board[open_x][open_y]){//開けるところの数字と周りのフラグが同じ数字ならフラグのないカバーを開ける
                    for(int i = 0; i < 8; i++){
                         int nx = open_x + dxs[i];
                         int ny = open_y + dys[i];

                        if(hanni(nx, ny) && cover_board[nx][ny] && !frag_board[nx][ny]){
                            cover_board[nx][ny] = false;
                            chainfunc(nx, ny);
                        }
                    }
                }
            }            
        }



        //frag
        void fragfunc(int frag_x, int frag_y){//フラグを置く
            if(hanni(frag_x, frag_y)){
                if(frag_board[frag_x][frag_y]){
                    frag_board[frag_x][frag_y] = false;
                }
                else{
                    frag_board[frag_x][frag_y] = true;
                }
            }
        }




    public:
        void actionfunc(int action_x, int action_y, int action){//
            switch(action){
                case 0:
                    openfunc(action_x, action_y);
                    break;
                case 1:
                    fragfunc(action_x, action_y);
                    break;
                default:
                    break;
            }
        }



    private:
        //連鎖
        void chainfunc(int open_x, int open_y){
            //0の周りのマスを開く 
            if(num_board[open_x][open_y] == 0 && !mine_board[open_x][open_y]){
                for(int i = 0; i < 8; i++){
                    int X = open_x + dxs[i];
                    int Y = open_y + dys[i];
                    if(hanni(X, Y)){
                        if(cover_board[X][Y]){
                            cover_board[X][Y] = false;
                            chainfunc(X, Y);//また0のマスが出てきたらまたchainfunc
                        }
                    }
                } 
            }
        }



    public:
        //地雷のマスが開いていないかどうか
        bool is_exploded(){
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    if(!cover_board[i][j] && mine_board[i][j]){
                        return true;
                    }
                }
            }
            return false;
        }

        //クリアしたかどうか
        bool is_cleared(){
            int covers = 0;
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    if(cover_board[i][j]){
                        covers++;
                    }
                }
            }

            if(covers == mine_num){
                return true;
            }
            else {
                return false;
            }
        }
    private:
        //表示用ボード
        void display_board_func(){
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    if(cover_board[i][j]){//カバーがあればカバーかフラグ
                        if(frag_board[i][j]){
                            display_board[i][j] = frag;
                        }
                        else {
                            display_board[i][j] = cover;
                        }
                    }
                    else if(mine_board[i][j]){
                         display_board[i][j] = mine;
                    }
                    else{
                        display_board[i][j] = num_board[i][j];
                    }
                }
            }

            //フラグの総数
            frag_num = 0;
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    if(frag_board[i][j]){
                        frag_num++;
                    }
                }
            }
        }
    
    
    
    public:
        //表示
        void display(){
            display_board_func();

            //std::cout << "Frag : x y 1 / OPEN : x y 0" << std::endl;
            std::cout << frag_num << "/" <<  mine_num << std::endl;
            
            //列番号
            std::cout << "x\\y";
            for(int i = 0; i < size; i++){
                std::cout << (i < 10 ? "|" : "") << i << '|';
            }
            std::cout << std::endl;

            for(int i = 0; i < size; i++){
                //行番号
                std::cout << (i < 10 ? "  " : " ") << i;
                
                //メイン
                for(int j = 0; j < size; j++){
                    if(display_board[i][j] == 0){
                        std::cout << " - ";
                    }
                    else if(display_board[i][j] < 9){
                        std::cout << ' ' << display_board[i][j] << ' ';
                    }
                    else if(display_board[i][j] == mine){
                        std::cout << " M ";
                    }
                    else if(display_board[i][j] == cover){
                        std::cout << "|_|";
                    }
                    else if(display_board[i][j] == frag){
                        std::cout << "|F|";
                    }
                    else{
                        std::cout << "|X|";
                    }

                }
                std::cout << std::endl;
            }
            
        }
       

};

//零号機
class minebot : public minesweaper{
  public:
    int around_mine_num;
    int around_frag_num;
    int around_cover_num;
    
    minebot()
    {
    }

    //フラグを立てる
    bool put_frag(int x, int y){
        bool changed = false;
        around_cover_num = 0;
        if(display_board[x][y] < 9){
            around_mine_num = display_board[x][y];
            for(int i = 0; i < 8; i++){
                int mx = x + dxs[i];
                int my = y + dys[i];
                if(hanni(mx, my) && (display_board[mx][my] == cover || display_board[mx][my] == frag)){
                    around_cover_num++;
                }
            }
            if(around_mine_num == around_cover_num){
                for(int i = 0; i < 8; i++){
                    int mx = x + dxs[i];
                    int my = y + dys[i];
                    if(hanni(mx, my) && display_board[mx][my] == cover){
                        actionfunc(mx, my, 1);
                        changed = true;
                    }
                }
            }
        }
        return changed;
    }

    bool open_cover(int x, int y){
        bool changed = false;
        around_frag_num = 0;
        around_cover_num = 0;
        if(display_board[x][y] < 9){
            around_mine_num = display_board[x][y];
            for(int i = 0; i < 8; i++){
                int mx = x + dxs[i];
                int my = y + dys[i];
                if(hanni(mx, my) && display_board[mx][my] == frag){
                    around_frag_num++;
                }
            
            }
            for(int i = 0; i < 8; i++){
                int mx = x + dxs[i];
                int my = y + dys[i];
                if(hanni(mx, my) && (display_board[mx][my] == cover || display_board[mx][my] == frag)){
                    around_cover_num++;
                }
            }
            if(around_mine_num == around_frag_num && around_cover_num > 0){
                for(int i = 0; i < 8; i++){
                    int mx = x + dxs[i];
                    int my = y + dys[i];
                    if(hanni(mx, my) && display_board[mx][my] == cover){
                        actionfunc(mx, my, 0);
                        changed = true;
                    }
                }
                
            }
        }
        return changed;
    }

    bool autoscan(){
        bool changed = false;
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(put_frag(i, j)){
                    display();
                    std::cout << i << " " << j << " frag" << std::endl << std::endl;
                    changed = true;
                    //Sleep(1000);
                }
            }
        }
        

        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(open_cover(i, j)){
                    display();
                    std::cout << i << " " << j << " open" << std::endl << std::endl;
                    changed = true; 
                    //Sleep(1000);
                }
            }
        }
        return changed;
    }

     
};

class minebot1:public minesweaper{
    public:
        int around_mine_num;
        int around_cover_num;
        int around_frag_num;
        std::vector<std::vector<bool>> mine_pattern;
        std::vector<std::vector<bool>> mine_pattern_sub;
        std::vector<std::vector<std::vector<std::vector<bool>>>> all_pattern;
        std::vector<std::vector<std::vector<int>>> possible_pattern;
        std::vector<int> pattern_memo;


        
    minebot1():
        mine_pattern(3, std::vector<bool>(3, false)),
        possible_pattern(size, std::vector<std::vector<int>>(size, std::vector<int>(0,0))),
        pattern_memo(0, 0)
        {
        auto mine_pattern_clear = mine_pattern;
        //all_patternの生成
        all_pattern.resize(9);

        for(int bit256 = 0; bit256 < 256; bit256++){//2^8のパターンを二進数で
            int mine_count = 0;
            int t = 1;
            mine_pattern = mine_pattern_clear;
            for(int i = 0; i < 8; i++){
                if((bit256 >> i) & 1){
                    int mx = 1 + dxs[i];
                    int my = 1 + dys[i];
                    mine_pattern[mx][my] = true;
                    mine_count++;
                }
            }
            all_pattern[mine_count].push_back(mine_pattern);
        }
    }

    bool hanni3x3(int x, int y){//範囲確認
        return 0 <= x && x < 3 && 0 <= y && y < 3;
    }

    void first_analysis(int x, int y){//あり得る地雷の配置
        around_mine_num = display_board[x][y];
        possible_pattern[x][y].clear();
        if(0 < around_mine_num && around_mine_num < mine){
            int pattern = all_pattern[around_mine_num].size();
            for(int i = 0; i < pattern; i++){
                bool possible = true;
                mine_pattern = all_pattern[around_mine_num][i];
                for(int j = 0; j < 8; j++){
                    //display_board
                    int mx = x + dxs[j];
                    int my = y + dys[j];
                    //mine_pattern
                    int nx = 1 + dxs[j];
                    int ny = 1 + dys[j];

                    if(!hanni(mx, my)){//範囲外に地雷を置くパターンを排除
                        if(mine_pattern[nx][ny]){
                            possible = false;
                        }
                    }
                    else{//開いたマスに地雷があるパターンとフラグがあるのに既に開かれているパターンを排除
                        if((mine_pattern[nx][ny] && display_board[mx][my] < mine) || (!mine_pattern[nx][ny] && display_board[mx][my] == frag)){
                            possible = false;
                        }

                    }
                }
                if(possible){
                    possible_pattern[x][y].push_back(i);
                }
            }
        }
    }




    void second_analysis(int x, int y){//となりの候補すべてと自分の候補に矛盾がある場合けす
        pattern_memo.clear();
        if(0 < display_board[x][y] && display_board[x][y] < mine){
            int pattern_num = possible_pattern[x][y].size();//自分の候補の要素数
            
            for(int i = 0; i < pattern_num; i++){//自分の候補
                bool possible = true;
                mine_pattern = all_pattern[display_board[x][y]][possible_pattern[x][y][i]];//各自分の候補
                
                for(int j = 0; j < 8; j++){//周りのマス
                    int mx = x + dxs[j];
                    int my = y + dys[j];

                    if(hanni(mx, my) && 0 < display_board[mx][my] && display_board[mx][my] < mine){
                        int pattern_num_sub = possible_pattern[mx][my].size();//各周りのマスの候補の要素数
                        bool possible1 = false;
                    
                        for(int k = 0; k < pattern_num_sub; k++){
                            mine_pattern_sub = all_pattern[display_board[mx][my]][possible_pattern[mx][my][k]];//各周りの候補
                            bool possible2 = true;

                            for(int l = 0; l < 8; l++){
                                int x1 = 1 + dxs[l];
                                int y1 = 1 + dys[l];

                                int x2 = 1 + dxs[l] - dxs[j];
                                int y2 = 1 + dys[l] - dys[j];

                                if((hanni3x3(x1, y1) && hanni3x3(x2, y2)) && mine_pattern[x1][y1] != mine_pattern_sub[x2][y2]){
                                    possible2 = false;//矛盾すれば不適
                                    break;
                                }
                            }
                            if(possible2){
                                possible1 = true;
                                break;
                            }
                            
                        }
                        if(!possible1){
                            possible = false;
                            break;
                        }
                    }
                }
                if(possible){
                    pattern_memo.push_back(possible_pattern[x][y][i]);
                }
            }
            possible_pattern[x][y] = pattern_memo;
        }
    }




    bool put_frag(int x, int y){
        bool changed = false;
        if(display_board[x][y] < 0 || mine < display_board[x][y]) {
            return false;
        }
        int pattern_num = possible_pattern[x][y].size();
        if(pattern_num == 0) {
            return false;
        }

        std::vector<std::vector<bool>> possible_frag(3, std::vector<bool>(3, true));//フラグを置く位置

        for(int i = 0; i < pattern_num; i++){
            mine_pattern = all_pattern[display_board[x][y]][possible_pattern[x][y][i]];

            for(int j = 0; j < 3; j++){//すべてのパターンで地雷がある場合フラグを置く
                for(int k = 0; k < 3; k++){
                    possible_frag[j][k] = possible_frag[j][k] && mine_pattern[j][k];
                }
            }
        }

        for(int i = 0; i < 8; i++){
            int mx = x + dxs[i];
            int my = y + dys[i];

            int nx = 1 + dxs[i];
            int ny = 1 + dys[i];
            if(hanni(mx, my) && possible_frag[nx][ny] && display_board[mx][my] == cover){//フラグを置く
                actionfunc(mx, my, 1);
                changed = true;
            }
        }
        return changed;
    }




    bool open_cover(int x, int y){
        bool changed = false;
        if(display_board[x][y] <= 0 || mine < display_board[x][y]) {
            return false;
        }
        int pattern_num = possible_pattern[x][y].size();
        if(pattern_num == 0){
            return false;
        }

        std::vector<std::vector<bool>> possible_mine(3, std::vector<bool>(3, false));

        for(int i = 0; i < pattern_num; i++){//すべてのパターンで地雷のがおかれない場所を開ける。
            mine_pattern = all_pattern[display_board[x][y]][possible_pattern[x][y][i]];
            for(int j = 0; j < 3; j++){
                for(int k = 0; k < 3; k++){
                    possible_mine[j][k] = possible_mine[j][k] || mine_pattern[j][k];
                }
            }
        }

        for(int i = 0; i < 8; i++){
            int mx = x + dxs[i];
            int my = y + dys[i];

            int nx = 1 + dxs[i];
            int ny = 1 + dys[i];
            if(hanni(mx, my) && !possible_mine[nx][ny] && display_board[mx][my] == cover){
                actionfunc(mx, my, 0);
                changed = true;
            }
        }

        return changed;
    }




    bool autoscan(){
        bool changed = false;
        //分析一回目
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                first_analysis(i, j);
            }
        }
        //分析二回目
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                second_analysis(i, j);
            }
        }
        //フラグを置く
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(put_frag(i, j)){
                    display();
                    std::cout << i << " " << j << " frag" << std::endl << std::endl;
                    changed = true;
                    Sleep(250);
                }
            }
        }
        //開ける
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(open_cover(i, j)){
                    display();
                    std::cout << i << " " << j << " open" << std::endl << std::endl;
                    changed = true;
                    Sleep(250);
                }
            }
        }
        return changed;

    }

    bool gamblefunc(int x, int y){
        bool changed = false;
        if(display_board[x][y] == cover){
            actionfunc(x, y, 0);
            changed = true;
        }
        return changed;
    }

    bool autogamble(){//論理的(フラグの総数を考えずに)に開けない場合左上のほうから開ける　フラグがあとひとつであればできそう？
        bool changed = false;
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(gamblefunc(i, j)){
                    display();
                    std::cout << i << " " << j << " GAMBLEGAMBLEGAMBLEGAMBLE" << std::endl << std::endl;
                    changed = true;
                    Sleep(250);
                    return changed;
                }
            }
        }
        return changed;
    }

};




void playergame(){
    minesweaper game;
    game.display();

    std::cout << game.mine_num << std::endl;
    int x;
    int y;
    int act;
    std::cin >> x >> y;
    //はじめに開く
    game.placemines(x, y);
    game.display();

    bool running = true;
    bool result = false;//勝敗
    while(running){
        std::cin >> x >> y >> act;
        game.actionfunc(x, y, act);
        if(game.is_exploded()){
            running = false;
            result = false;
            break;
        }
        if(game.is_cleared()){
            running = false;
            result = true;
            break;
        }
        game.display();
    }
    if(result){
        std::cout << "CLEAR!";
    }
    else{
        std::cout << "GAME OVER!";
    }

}


void botgame(){
    minebot game;

    game.placemines(game.size/2, game.size/2);
    game.display();

    bool running = true;
    bool changed = false;
    bool  result = false;//勝敗
    while(running){
        changed = false;
        changed = game.autoscan();

        if(game.is_exploded()){
            running = false;
            result = false;
        }
        if(game.is_cleared()){
            running = false;
            result = true; 
        }
        if(!changed){
            running = false;
        }
        
    }
    if(result){
        std::cout << "CLEAR!" << std::endl;
    }
    else{
        std::cout << "GAME OVER!" << std::endl;
    }
}


void botgame1(){
    minebot1 game;

    game.placemines(game.size/2, game.size/2);
    game.mine_display();
    game.display();
    //Sleep(2500);

    bool running = true;
    bool changed = false;
    bool  result = false;//勝敗
    while(running){
        if(game.is_exploded()){
            running = false;
            result = false;
            break;
        }
        if(game.is_cleared()){
            running = false;
            result = true; 
            break;
        }
        
        changed = false;
        changed = game.autoscan();
        
          if(!changed){
            game.autogamble();
        }
        
    }
    if(result){
        std::cout << "CLEAR!" << std::endl;
    }
    else{
        std::cout << "GAME OVER!" << std::endl;
    }


}


int main(){
    botgame1();
    int a;
    std::cout << "END" << std::endl;
    std::cin >> a;
}