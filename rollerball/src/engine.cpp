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

int64_t totalnodes = 0;

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

bool ge_operator(const std::pair<int64_t,int16_t> a, const std::pair<int64_t,int16_t> b){
    if(a.first != b.first){
        return (a.first > b.first);
    }
    else if(a.first < 0){
        return (a.second >= b.second);
    }
    
    return (a.second <= b.second);
    
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

    if (promo == PAWN_ROOK) {
        piecetype = ((piecetype & (WHITE | BLACK)) ^ ROOK) | PAWN;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = ((piecetype & (WHITE | BLACK)) ^ BISHOP) | PAWN;
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

const int64_t MAX = 100000;
const int64_t MIN = -100000;

int64_t attacking_nature = 50;
int64_t defending_nature = 50;

std::vector<std::string> prev_boards;


int64_t heuristic(Board& b) {
    int64_t score = 0;

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

        // opponent pieces
        if(b.in_check()){
            score += 100 * attacking_nature;
        }
        if(b.data.b_rook_ws != DEAD){
            score -= 20 * attacking_nature;
            if(under_threat(my_attack_positions,b.data.b_rook_ws)){
                score += 5 * attacking_nature;
            }
        }
        if(b.data.b_rook_bs != DEAD){
            score -= 20 * attacking_nature;
            if(under_threat(my_attack_positions,b.data.b_rook_bs)){
                score += 5 * attacking_nature;
            }
        }
        if(b.data.b_bishop != DEAD){
            score -= 20 * attacking_nature;
            if(under_threat(my_attack_positions,b.data.b_bishop)){
                score += 3 * attacking_nature;
            }
        }
        if(b.data.b_pawn_ws != DEAD){
            auto piecetype = b.data.board_0[b.data.b_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.b_pawn_ws)){
                   score += 5 * attacking_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.b_pawn_ws)){
                   score += 3 * attacking_nature;
                }
            }
            else{
                score -= 10 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.b_pawn_ws)){
                   score += 1 * attacking_nature;
                }
            }
            
        }
        if(b.data.b_pawn_bs != DEAD){
            auto piecetype = b.data.board_0[b.data.b_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.b_pawn_bs)){
                   score += 5 * attacking_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.b_pawn_bs)){
                   score += 3 * attacking_nature;
                }
            }
            else{
                score -= 10 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.b_pawn_bs)){
                   score += 1 * attacking_nature;
                }
            }     
        }

        // my pieces
        if(b.data.w_rook_ws != DEAD){
            score += 20 * defending_nature;
            if(under_threat(opp_attack_positions,b.data.w_rook_ws)){
                score -= 5 * defending_nature;
            }
        }
        if(b.data.w_rook_bs != DEAD){
            score += 20 * defending_nature;
            if(under_threat(opp_attack_positions,b.data.w_rook_bs)){
                score -= 5 * defending_nature;
            }
        }
        if(b.data.w_bishop != DEAD){
            score += 20 * defending_nature;
            if(under_threat(opp_attack_positions,b.data.w_bishop)){
                score -= 3 * defending_nature;
            }
        }
        if(b.data.w_pawn_ws != DEAD){
            auto piecetype = b.data.board_0[b.data.w_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.w_pawn_ws)){
                   score -= 5 * defending_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.w_pawn_ws)){
                   score -= 3 * defending_nature;
                }
            }
            else{
                score += 10 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.w_pawn_ws)){
                   score -= 1 * defending_nature;
                }
            }
        }
        if(b.data.w_pawn_bs != DEAD){
            auto piecetype = b.data.board_0[b.data.w_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.w_pawn_bs)){
                   score -= 5 * defending_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.w_pawn_bs)){
                   score -= 3 * defending_nature;
                }
            }
            else{
                score += 10 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.w_pawn_bs)){
                   score -= 1 * defending_nature;
                }
            }     
        }


    }
    else{

        // opponent pieces
        if(b.in_check()){
            score += 100 * attacking_nature;
        }
        if(b.data.w_rook_ws != DEAD){
            score -= 20 * attacking_nature;
            if(under_threat(my_attack_positions,b.data.w_rook_ws)){
                score += 5 * attacking_nature;
            }
        }
        if(b.data.w_rook_bs != DEAD){
            score -= 20 * attacking_nature;
            if(under_threat(my_attack_positions,b.data.w_rook_bs)){
                score += 5 * attacking_nature;
            }
        }
        if(b.data.w_bishop != DEAD){
            score -= 20 * attacking_nature;
            if(under_threat(my_attack_positions,b.data.w_bishop)){
                score += 3 * attacking_nature;
            }
        }
        if(b.data.w_pawn_ws != DEAD){
            auto piecetype = b.data.board_0[b.data.w_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.w_pawn_ws)){
                   score += 5 * attacking_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.w_pawn_ws)){
                   score += 3 * attacking_nature;
                }
            }
            else{
                score -= 10 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.w_pawn_ws)){
                   score += 1 * attacking_nature;
                }
            }
        }
        if(b.data.w_pawn_bs != DEAD){
            auto piecetype = b.data.board_0[b.data.w_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.w_pawn_bs)){
                   score += 5 * attacking_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score -= 20 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.w_pawn_bs)){
                   score += 3 * attacking_nature;
                }
            }
            else{
                score -= 10 * attacking_nature;
                if(under_threat(my_attack_positions,b.data.w_pawn_bs)){
                   score += 1 * attacking_nature;
                }
            }  
        }

        // my pieces
        if(b.data.b_rook_ws != DEAD){
            score += 20 * defending_nature;
            if(under_threat(opp_attack_positions,b.data.b_rook_ws)){
                score -= 5 * defending_nature;
            }
        }
        if(b.data.b_rook_bs != DEAD){
            score += 20 * defending_nature;
            if(under_threat(opp_attack_positions,b.data.b_rook_bs)){
                score -= 5 * defending_nature;
            }
        }
        if(b.data.b_bishop != DEAD){
            score += 20 * defending_nature;
            if(under_threat(opp_attack_positions,b.data.b_bishop)){
                score -= 3 * defending_nature;
            }
        }
        if(b.data.b_pawn_ws != DEAD){
            auto piecetype = b.data.board_0[b.data.b_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.b_pawn_ws)){
                   score -= 5 * defending_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.b_pawn_ws)){
                   score -= 3 * defending_nature;
                }
            }
            else{
                score += 10 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.b_pawn_ws)){
                   score -= 1 * defending_nature;
                }
            }
        }
        if(b.data.b_pawn_bs != DEAD){
            auto piecetype = b.data.board_0[b.data.b_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.b_pawn_bs)){
                   score -= 5 * defending_nature;
                }
            }
            else if((piecetype & BISHOP) == piecetype){
                score += 20 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.b_pawn_bs)){
                   score -= 3 * defending_nature;
                }
            }
            else{
                score += 10 * defending_nature;
                if(under_threat(opp_attack_positions,b.data.b_pawn_bs)){
                   score -= 1 * defending_nature;
                }
            }
        }

    }
    return score;
}

int64_t draw_heuristic(Board &b){
    int64_t want_to_draw = 0;
    if(b.data.player_to_play == WHITE){
        if(b.data.b_rook_ws != DEAD){
            want_to_draw += 20;
        }
        if(b.data.b_rook_bs != DEAD){
            want_to_draw += 20;
        }
        if(b.data.b_bishop != DEAD){
            want_to_draw += 20;
        }
        if(b.data.b_pawn_ws != DEAD){
            want_to_draw += 10;
            auto piecetype = b.data.board_0[b.data.b_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                want_to_draw += 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw += 10;
            }
        }
        if(b.data.b_pawn_bs != DEAD){
            want_to_draw += 10;
            auto piecetype = b.data.board_0[b.data.b_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                want_to_draw += 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw += 10;
            }
        }
        if(b.data.w_rook_ws != DEAD){
            want_to_draw -= 20;
        }
        if(b.data.w_rook_bs != DEAD){
            want_to_draw -= 20;
        }
        if(b.data.w_bishop != DEAD){
            want_to_draw -= 20;
        }
        if(b.data.w_pawn_ws != DEAD){
            want_to_draw -= 10;
            auto piecetype = b.data.board_0[b.data.w_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                want_to_draw -= 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw -= 10;
            }
        }
        if(b.data.w_pawn_bs != DEAD){
            want_to_draw -= 10;
            auto piecetype = b.data.board_0[b.data.w_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                want_to_draw -= 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw -= 10;
            }
        }

    }
    else{
        if(b.data.w_rook_ws != DEAD){
            want_to_draw += 20;
        }
        if(b.data.w_rook_bs != DEAD){
            want_to_draw += 20;
        }
        if(b.data.w_bishop != DEAD){
            want_to_draw += 20;
        }
        if(b.data.w_pawn_ws != DEAD){
            want_to_draw += 10;
            auto piecetype = b.data.board_0[b.data.w_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                want_to_draw += 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw += 10;
            }
        }
        if(b.data.w_pawn_bs != DEAD){
            want_to_draw += 10;
            auto piecetype = b.data.board_0[b.data.w_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                want_to_draw += 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw += 10;
            }
        }
        if(b.data.b_rook_ws != DEAD){
            want_to_draw -= 20;
        }
        if(b.data.b_rook_bs != DEAD){
            want_to_draw -= 20;
        }
        if(b.data.b_bishop != DEAD){
            want_to_draw -= 20;
        }
        if(b.data.b_pawn_ws != DEAD){
            want_to_draw -= 10;
            auto piecetype = b.data.board_0[b.data.b_pawn_ws];
            if((piecetype & ROOK) == piecetype){
                want_to_draw -= 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw -= 10;
            }
        }
        if(b.data.b_pawn_bs != DEAD){
            want_to_draw -= 10;
            auto piecetype = b.data.board_0[b.data.b_pawn_bs];
            if((piecetype & ROOK) == piecetype){
                want_to_draw -= 10;
            }
            else if((piecetype & BISHOP) == piecetype){
                want_to_draw -= 10;
            }
        }

    }
    return want_to_draw * 100;
}

std::pair<std::pair<int64_t, int16_t>,U16> minimax(Board &b,int16_t depth,
            bool maximizingPlayer,
            std::pair<int64_t, int16_t>  alpha,
            std::pair<int64_t, int16_t> beta,
            std::pair<U8,int> last_killed_data)
{
    // Terminating condition. i.e
    // leaf node is reached
    auto moves = b.get_legal_moves();
    if(moves.size() == 0 && b.in_check()) {
        if(maximizingPlayer)
            return std::make_pair(std::make_pair(MIN,depth),0);
        else
            return std::make_pair(std::make_pair(MAX,depth),0);
    }
    else if(moves.size() == 0){
        if(maximizingPlayer)
            return std::make_pair(std::make_pair(draw_heuristic(b),depth),0);
        else
            return std::make_pair(std::make_pair(-draw_heuristic(b),depth),0);
    }

    if (depth == 3) {
        auto value = heuristic(b);
        if(depth % 2 == 0){
            value = -value;
        }
        return std::make_pair(std::make_pair(value,depth), 0);
    }
 

    if (maximizingPlayer)
    {
        std::pair<std::pair<int64_t, int16_t>, U16>  best = {{MIN,-1}, *moves.begin()};
 
        // Recur for left and
        // right children
        for (auto m : moves) {
            totalnodes++;
            
            std::string board_str = all_boards_to_str(b);
            b.do_move(m);
            std::pair<std::pair<int64_t, int16_t>, U16> val;
            prev_boards.emplace_back(board_encode(b));
            if(std::count(prev_boards.begin(),prev_boards.end(),board_encode(b)) == 3){
                val = {{-draw_heuristic(b),depth},0}; //
            }
            else{
                val = minimax(b, depth + 1,
                                false, alpha, beta,std::make_pair(b.data.last_killed_piece, b.data.last_killed_piece_idx));
            }
            prev_boards.pop_back();

            b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));

            undo_last_move(b,m);
            std::string board_str1 = all_boards_to_str(b);    
            assert(board_str == board_str1);
            b.data.last_killed_piece = last_killed_data.first;
            b.data.last_killed_piece_idx = last_killed_data.second;
            
            if(ge_operator(val.first,best.first)) {
                best.first = val.first;
                best.second = m;
            }
            if(ge_operator(best.first,alpha))
                alpha = best.first; 
 
            // Alpha Beta Pruning
            if (ge_operator(alpha,beta))
                break;
        }
        
        return best;
    }
    else
    {
        std::pair<std::pair<int64_t, int16_t>, U16>  best = {{MAX,-1}, *moves.begin()};

        for (auto m : moves) {
            totalnodes++;
            std::string board_str = all_boards_to_str(b);
            b.do_move(m);
            prev_boards.emplace_back(board_encode(b));
            std::pair<std::pair<int64_t, int16_t>, U16> val;
            if(std::count(prev_boards.begin(),prev_boards.end(),board_encode(b)) == 3){
                val = {{draw_heuristic(b), depth}, 0}; 
            }
            else{
                val = minimax(b, depth + 1,
                                true, alpha, beta,std::make_pair(b.data.last_killed_piece, b.data.last_killed_piece_idx));
            }
            prev_boards.pop_back();
            b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
            
            undo_last_move(b,m);
            std::string board_str1 = all_boards_to_str(b);    
            assert(board_str == board_str1);
            b.data.last_killed_piece = last_killed_data.first;
            b.data.last_killed_piece_idx = last_killed_data.second;
            

            if(ge_operator(best.first,val.first)) {
                best.first = val.first;
                best.second = m;
            }
            if(ge_operator(beta,best.first))
                beta = best.first;
 
            // Alpha Beta Pruning
            if (ge_operator(alpha,beta))
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
        prev_boards.emplace_back(board_encode(b));
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

        totalnodes = 0;

        auto search_result = minimax(search_board, 0, true, std::make_pair(MIN,-1), std::make_pair(MAX,-1),std::make_pair(b.data.last_killed_piece, b.data.last_killed_piece_idx));

        std::cout<<"Total nodes: "<<totalnodes<<std::endl;

        this->best_move = search_result.second;

        assert(all_boards_to_str(b) == all_boards_to_str(search_board));
        assert(board_encode(b) == board_encode(search_board));

        std::cout<<search_result.first.first<<" "<<search_result.first.second<<std::endl;

        if(this->best_move != 0){
            search_board.do_move(this->best_move);
            if(heuristic(search_board) < 0){
                if(attacking_nature > defending_nature){
                    std::swap(attacking_nature,defending_nature);
                }
                else{
                    attacking_nature-=2;
                    defending_nature+=2;
                    attacking_nature = std::max(attacking_nature,int64_t(48));
                    defending_nature = std::min(defending_nature,int64_t(52));
                }
            }
            else{
                if(attacking_nature < defending_nature){
                    std::swap(attacking_nature,defending_nature);
                }
                else{
                    attacking_nature+=2;
                    defending_nature-=2;
                    defending_nature = std::max(defending_nature,int64_t(48));
                    attacking_nature = std::min(attacking_nature,int64_t(52));
                }
            }
            prev_boards.emplace_back(board_encode(search_board));
        }
    }
    return;
}
