#include <algorithm>
#include <random>
#include <iostream>

#include "board.hpp"
#include "engine.hpp"

const long long MAX = 1000;
const long long MIN = -1000;

std::vector<std::string> prev_boards;

long long heuristic(const Board& b) {
    long long score = 0;
    if(b.data.player_to_play == BLACK) {
        if(b._under_threat(b.data.b_king)){
            score += 100;
        }
        if(b.data.b_rook_ws == DEAD){
            score += 20;
        }
        else if(b._under_threat(b.data.b_rook_ws)){
            score += 5;
        }
        if(b.data.b_rook_bs == DEAD){
            score += 20;
        }
        else if(b._under_threat(b.data.b_rook_bs)){
            score += 5;
        }
        if(b.data.b_bishop == DEAD){
            score += 20;
        }
        else if(b._under_threat(b.data.b_bishop)){
            score += 3;
        }
        if(b.data.b_pawn_ws == DEAD){
            score += 10;
        }
        else if(b._under_threat(b.data.b_pawn_ws)){
            score += 1;
        }
        if(b.data.b_pawn_bs == DEAD){
            score += 10;
        }
        else if(b._under_threat(b.data.b_pawn_bs)){
            score += 1;
        }      
        if(b._under_threat(b.data.w_king)){
            score -= 100;
        }
        if(b.data.w_rook_ws == DEAD){
            score -= 20;
        }
        else if(b._under_threat(b.data.w_rook_ws)){
            score -= 5;
        }
        if(b.data.w_rook_bs == DEAD){
            score -= 20;
        }
        else if(b._under_threat(b.data.w_rook_bs)){
            score -= 5;
        }
        if(b.data.w_bishop == DEAD){
            score -= 20;
        }
        else if(b._under_threat(b.data.w_bishop)){
            score -= 3;
        }
        if(b.data.w_pawn_ws == DEAD){
            score -= 10;
        }
        else if(b._under_threat(b.data.w_pawn_ws)){
            score -= 1;
        }
        if(b.data.w_pawn_bs == DEAD){
            score -= 10;
        }
        else if(b._under_threat(b.data.w_pawn_bs)){
            score -= 1;
        }        


    }
    else{
        if(b._under_threat(b.data.w_king)){
            score += 100;
        }
        if(b.data.w_rook_ws == DEAD){
            score += 20;
        }
        else if(b._under_threat(b.data.w_rook_ws)){
            score += 5;
        }
        if(b.data.w_rook_bs == DEAD){
            score += 20;
        }
        else if(b._under_threat(b.data.w_rook_bs)){
            score += 5;
        }
        if(b.data.w_bishop == DEAD){
            score += 20;
        }
        else if(b._under_threat(b.data.w_bishop)){
            score += 3;
        }
        if(b.data.w_pawn_ws == DEAD){
            score += 10;
        }
        else if(b._under_threat(b.data.w_pawn_ws)){
            score += 1;
        }
        if(b.data.w_pawn_bs == DEAD){
            score += 10;
        }
        else if(b._under_threat(b.data.w_pawn_bs)){
            score += 1;
        }      
        if(b._under_threat(b.data.b_king)){
            score -= 100;
        }
        if(b.data.b_rook_ws == DEAD){
            score -= 20;
        }
        else if(b._under_threat(b.data.b_rook_ws)){
            score -= 5;
        }
        if(b.data.b_rook_bs == DEAD){
            score -= 20;
        }
        else if(b._under_threat(b.data.b_rook_bs)){
            score -= 5;
        }
        if(b.data.b_bishop == DEAD){
            score -= 20;
        }
        else if(b._under_threat(b.data.b_bishop)){
            score -= 3;
        }
        if(b.data.b_pawn_ws == DEAD){
            score -= 10;
        }
        else if(b._under_threat(b.data.b_pawn_ws)){
            score -= 1;
        }
        if(b.data.b_pawn_bs == DEAD){
            score -= 10;
        }
        else if(b._under_threat(b.data.b_pawn_bs)){
            score -= 1;
        }        
    }
    return score;
}

std::pair<std::pair<long long, short int>,U16> minimax(Board b,int depth,
            bool maximizingPlayer,
            long long  alpha,
            long long beta)
{
    // Terminating condition. i.e
    // leaf node is reached
    auto moves = b.get_legal_moves();
    if(moves.size() == 0) {
        return (maximizingPlayer) ? std::make_pair(std::make_pair(MIN,-depth),0) : std::make_pair(std::make_pair(MAX,-depth),0);
    }
    if (depth == 3) {
        all_boards_to_str(b);
        return {{heuristic(b),-depth}, 0};
    }
 

    if (maximizingPlayer)
    {
        std::pair<std::pair<long long, short int>, U16>  best = {{MIN,100}, *moves.begin()};
 
        // Recur for left and
        // right children
        for (auto m : moves) {
            
            b._do_move(m);
            b._flip_player();
            std::pair<std::pair<long long, short int>, U16> val;
            prev_boards.push_back(all_boards_to_str(b));
            if(std::count(prev_boards.begin(),prev_boards.end(),all_boards_to_str(b)) == 2){
                val = {{-300,-depth},0}; // TODO: Draw heuristic
            }
            else{
                val = minimax(b, depth + 1,
                                false, alpha, beta);
            }
            prev_boards.pop_back();
            b._undo_last_move(m);
            b._flip_player();
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
            
            b._do_move(m);
            b._flip_player();
            prev_boards.push_back(all_boards_to_str(b));
            std::pair<std::pair<long long, short int>, U16> val;
            if(std::count(prev_boards.begin(),prev_boards.end(),all_boards_to_str(b)) == 2){
                val = {{-300, -depth}, 0}; // Adversary is a pussy
            }
            else{
                val = minimax(b, depth + 1,
                                true, alpha, beta);
            }
            prev_boards.pop_back();
            b._undo_last_move(m);
            b._flip_player();
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
        prev_boards.push_back(all_boards_to_str(b));
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

        this->best_move = minimax(search_board, 0, true, MIN, MAX).second;

        if(this->best_move != 0){
            search_board._do_move(this->best_move);
            prev_boards.push_back(all_boards_to_str(search_board));
        }

    }
}
