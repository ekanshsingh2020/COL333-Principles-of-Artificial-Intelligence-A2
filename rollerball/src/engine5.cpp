#include <algorithm>
#include <random>
#include <iostream>
#include<assert.h>

#include "board.hpp"
#include "engine.hpp"

constexpr U8 cw_90[64] = {
    48, 40, 32, 24, 16, 8,  0,  7,
    49, 41, 33, 25, 17, 9,  1,  15,
    50, 42, 18, 19, 20, 10, 2,  23,
    51, 43, 26, 27, 28, 11, 3,  31,
    52, 44, 34, 35, 36, 12, 4,  39,
    53, 45, 37, 29, 21, 13, 5,  47,
    54, 46, 38, 30, 22, 14, 6,  55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 acw_90[64] = {
     6, 14, 22, 30, 38, 46, 54, 7,
     5, 13, 21, 29, 37, 45, 53, 15,
     4, 12, 18, 19, 20, 44, 52, 23,
     3, 11, 26, 27, 28, 43, 51, 31,
     2, 10, 34, 35, 36, 42, 50, 39,
     1,  9, 17, 25, 33, 41, 49, 47,
     0,  8, 16, 24, 32, 40, 48, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 cw_180[64] = {
    54, 53, 52, 51, 50, 49, 48, 7,
    46, 45, 44, 43, 42, 41, 40, 15,
    38, 37, 18, 19, 20, 33, 32, 23,
    30, 29, 26, 27, 28, 25, 24, 31,
    22, 21, 34, 35, 36, 17, 16, 39,
    14, 13, 12, 11, 10,  9,  8, 47,
     6,  5,  4,  3,  2,  1,  0, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 id[64] = {
     0,  1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

std::string board_encode(const Board& b){
    std::string encoding = "";

    encoding += std::to_string(getx(b.data.w_king));
    encoding += std::to_string(gety(b.data.w_king));
    encoding += std::to_string(getx(b.data.w_rook_bs));
    encoding += std::to_string(gety(b.data.w_rook_bs));
    encoding += std::to_string(getx(b.data.w_rook_ws));
    encoding += std::to_string(gety(b.data.w_rook_ws));
    encoding += std::to_string(getx(b.data.w_bishop));
    encoding += std::to_string(gety(b.data.w_bishop));
    encoding += std::to_string(getx(b.data.w_pawn_bs));
    encoding += std::to_string(gety(b.data.w_pawn_bs));
    encoding += std::to_string(getx(b.data.w_pawn_ws));
    encoding += std::to_string(gety(b.data.w_pawn_ws));

    encoding += std::to_string(getx(b.data.b_king));
    encoding += std::to_string(gety(b.data.b_king));
    encoding += std::to_string(getx(b.data.b_rook_bs));
    encoding += std::to_string(gety(b.data.b_rook_bs));
    encoding += std::to_string(getx(b.data.b_rook_ws));
    encoding += std::to_string(gety(b.data.b_rook_ws));
    encoding += std::to_string(getx(b.data.b_bishop));
    encoding += std::to_string(gety(b.data.b_bishop));
    encoding += std::to_string(getx(b.data.b_pawn_bs));
    encoding += std::to_string(gety(b.data.b_pawn_bs));
    encoding += std::to_string(getx(b.data.b_pawn_ws));
    encoding += std::to_string(gety(b.data.b_pawn_ws));

    return encoding;
}

bool under_threat(std::vector<U8> &opp_legal_moves, U8 piece_pos) {

    if(std::count(opp_legal_moves.begin(),opp_legal_moves.end(), piece_pos)) {
        return true;
    }

    return false;

}
void undo_last_move(Board &b,U16 move) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);

    U8 piecetype = b.data.board_0[p1];
    U8 deadpiece = b.data.last_killed_piece;

    // scan and get piece from coord
    U8 *pieces = (U8*)(&(b.data));
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = p0;
            break;
        }
    }
    if (b.data.last_killed_piece_idx >= 0) {
        pieces[b.data.last_killed_piece_idx] = p1;
    }

    if (promo == PAWN_ROOK || promo == PAWN_BISHOP) {
        piecetype = (piecetype & (WHITE | BLACK)) | PAWN;
    }
    

    b.data.board_0[p0]           = piecetype;
    b.data.board_90[cw_90[p0]]   = piecetype;
    b.data.board_180[cw_180[p0]] = piecetype;
    b.data.board_270[acw_90[p0]] = piecetype;

    b.data.board_0[p1]           = deadpiece;
    b.data.board_90[cw_90[p1]]   = deadpiece;
    b.data.board_180[cw_180[p1]] = deadpiece;
    b.data.board_270[acw_90[p1]] = deadpiece;



    return;

}

const long long MAX = 1000;
const long long MIN = -1000;

std::vector<std::string> prev_boards;


long long heuristic(Board& b) {
    long long score = 0;

    auto opp_legal_moves = b.get_legal_moves();
    
    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
    
    auto my_legal_moves = b.get_legal_moves();

    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));

    std::vector<U8> opp_attack_positions, my_attack_positions;

    for (auto move : my_legal_moves) {
        my_attack_positions.emplace_back(getp1(move));
    }
    for (auto move : opp_legal_moves) {
        opp_attack_positions.emplace_back(getp1(move));
    }

    if(b.data.player_to_play == BLACK) {
        if(under_threat(my_attack_positions,b.data.b_king)){
            score += 100;
        }
        if(b.data.b_rook_ws == DEAD){
            score += 20;
        }
        else if(under_threat(my_attack_positions,b.data.b_rook_ws)){
            score += 5;
        }
        if(b.data.b_rook_bs == DEAD){
            score += 20;
        }
        else if(under_threat(my_attack_positions,b.data.b_rook_bs)){
            score += 5;
        }
        if(b.data.b_bishop == DEAD){
            score += 20;
        }
        else if(under_threat(my_attack_positions,b.data.b_bishop)){
            score += 3;
        }
        if(b.data.b_pawn_ws == DEAD){
            score += 10;
        }
        else if(under_threat(my_attack_positions,b.data.b_pawn_ws)){
            score += 1;
        }
        if(b.data.b_pawn_bs == DEAD){
            score += 10;
        }
        else if(under_threat(my_attack_positions,b.data.b_pawn_bs)){
            score += 1;
        }      
        // if(under_threat(opp_attack_positions,b.data.w_king)){ // Should never happen
        //     score -= 100;
        // }
        if(b.data.w_rook_ws == DEAD){
            score -= 20;
        }
        else if(under_threat(opp_attack_positions,b.data.w_rook_ws)){
            score -= 5;
        }
        if(b.data.w_rook_bs == DEAD){
            score -= 20;
        }
        else if(under_threat(opp_attack_positions,b.data.w_rook_bs)){
            score -= 5;
        }
        if(b.data.w_bishop == DEAD){
            score -= 20;
        }
        else if(under_threat(opp_attack_positions,b.data.w_bishop)){
            score -= 3;
        }
        if(b.data.w_pawn_ws == DEAD){
            score -= 10;
        }
        else if(under_threat(opp_attack_positions,b.data.w_pawn_ws)){
            score -= 1;
        }
        if(b.data.w_pawn_bs == DEAD){
            score -= 10;
        }
        else if(under_threat(opp_attack_positions,b.data.w_pawn_bs)){
            score -= 1;
        }        


    }
    else{
        if(under_threat(my_attack_positions,b.data.w_king)){
            score += 100;
        }
        if(b.data.w_rook_ws == DEAD){
            score += 20;
        }
        else if(under_threat(my_attack_positions,b.data.w_rook_ws)){
            score += 5;
        }
        if(b.data.w_rook_bs == DEAD){
            score += 20;
        }
        else if(under_threat(my_attack_positions,b.data.w_rook_bs)){
            score += 5;
        }
        if(b.data.w_bishop == DEAD){
            score += 20;
        }
        else if(under_threat(my_attack_positions,b.data.w_bishop)){
            score += 3;
        }
        if(b.data.w_pawn_ws == DEAD){
            score += 10;
        }
        else if(under_threat(my_attack_positions,b.data.w_pawn_ws)){
            score += 1;
        }
        if(b.data.w_pawn_bs == DEAD){
            score += 10;
        }
        else if(under_threat(my_attack_positions,b.data.w_pawn_bs)){
            score += 1;
        }      
        // if(under_threat(opp_attack_positions,b.data.b_king)){ // Should never happen
        //     score -= 100;
        // }
        if(b.data.b_rook_ws == DEAD){
            score -= 20;
        }
        else if(under_threat(opp_attack_positions,b.data.b_rook_ws)){
            score -= 5;
        }
        if(b.data.b_rook_bs == DEAD){
            score -= 20;
        }
        else if(under_threat(opp_attack_positions,b.data.b_rook_bs)){
            score -= 5;
        }
        if(b.data.b_bishop == DEAD){
            score -= 20;
        }
        else if(under_threat(opp_attack_positions,b.data.b_bishop)){
            score -= 3;
        }
        if(b.data.b_pawn_ws == DEAD){
            score -= 10;
        }
        else if(under_threat(opp_attack_positions,b.data.b_pawn_ws)){
            score -= 1;
        }
        if(b.data.b_pawn_bs == DEAD){
            score -= 10;
        }
        else if(under_threat(opp_attack_positions,b.data.b_pawn_bs)){
            score -= 1;
        }        
    }
    return score;
}

std::pair<std::pair<long long, short int>,U16> minimax(Board &b,int depth,
            bool maximizingPlayer,
            long long  alpha,
            long long beta,
            std::pair<U8,int> last_killed_data)
{
    // Terminating condition. i.e
    // leaf node is reached
    auto moves = b.get_legal_moves();
    if(moves.size() == 0 && b.in_check()) {
        return (maximizingPlayer) ? std::make_pair(std::make_pair(MIN,-depth),0) : std::make_pair(std::make_pair(MAX,-depth),0);
    }
    else if(moves.size() == 0){
        return (maximizingPlayer) ? std::make_pair(std::make_pair(-300,-depth),0) : std::make_pair(std::make_pair(-300,-depth),0);
    }

    if (depth == 5) {
        return {{heuristic(b),-depth}, 0};
    }
 

    if (maximizingPlayer)
    {
        std::pair<std::pair<long long, short int>, U16>  best = {{MIN,100}, *moves.begin()};
 
        // Recur for left and
        // right children
        for (auto m : moves) {
            
            b.do_move(m);
            std::pair<std::pair<long long, short int>, U16> val;
            prev_boards.push_back(board_encode(b));
            if(std::count(prev_boards.begin(),prev_boards.end(),board_encode(b)) == 2){
                val = {{-300,-depth},0}; // TODO: Draw heuristic
            }
            else{
                val = minimax(b, depth + 1,
                                false, alpha, beta,std::make_pair(b.data.last_killed_piece, b.data.last_killed_piece_idx));
            }
            prev_boards.pop_back();

            b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));

            undo_last_move(b,m);
            
            b.data.last_killed_piece = last_killed_data.first;
            b.data.last_killed_piece_idx = last_killed_data.second;
            
            if(best < val) {
                best = val;
                best.second = m;
            }
            alpha = std::max(alpha, best.first.first);
 
            // Alpha Beta Pruning
            if (beta <= alpha)
                break;
        }
        
        return best;
    }
    else
    {
        std::pair<std::pair<long long, short int>, U16>  best = {{MAX,-100}, *moves.begin()};

        for (auto m : moves) {
            
            b.do_move(m);
            prev_boards.push_back(board_encode(b));
            std::pair<std::pair<long long, short int>, U16> val;
            if(std::count(prev_boards.begin(),prev_boards.end(),board_encode(b)) == 2){
                val = {{-300, -depth}, 0}; // Adversary is a pussy
            }
            else{
                val = minimax(b, depth + 1,
                                true, alpha, beta,std::make_pair(b.data.last_killed_piece, b.data.last_killed_piece_idx));
            }
            prev_boards.pop_back();
            b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
            
            undo_last_move(b,m);
            
            b.data.last_killed_piece = last_killed_data.first;
            b.data.last_killed_piece_idx = last_killed_data.second;
            

            if(best > val) {
                best = val;
                best.second = m;
            }
            beta = std::min(beta, best.first.first);
 
            // Alpha Beta Pruning
            if (beta <= alpha)
                break;
        }
        
        return best;
    }
}

void Engine::find_best_move(const Board& b) {

    // pick a random move
    
    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        this->best_move = 0;
    }
    else {
        Board search_board = b;
        prev_boards.push_back(board_encode(b));
        std::vector<U16> moves;

        std::cout << std::endl;
        std::sample(
            moveset.begin(),
            moveset.end(),
            std::back_inserter(moves),
            1,
            std::mt19937{std::random_device{}()}
        );

        this->best_move = moves[0];

        this->best_move = minimax(search_board, 0, true, MIN, MAX,std::make_pair(b.data.last_killed_piece, b.data.last_killed_piece_idx)).second;


        if(this->best_move != 0){
            search_board.do_move(this->best_move);
            prev_boards.push_back(board_encode(search_board));
        }

    }
    return;
}

