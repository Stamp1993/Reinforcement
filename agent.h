#include<iostream>
#include<random>
#include<cstdlib>
#include <windows.h>
#include<string>
#include<cmath>
using namespace std;



class mazeAgent  {//abstract mazes agent class
protected:
	enum cells { S, F, H, G };

	enum action { Up, Down, Left, Right};

	enum moveType { Greedy, epsilonGreedy, softmax };

	action op(action act) {//return opposite action
		if (act == Up) {
			return Down;
		}
		if (act == Down) {
			return Up;
		}
		if (act == Left) {
			return Right;
		}
		if (act == Right) {
			return Left;
		}
	}

	struct state {//two element pair for state allocation and its value
		int x;
		int y;
		cells value;

		state(int a, int b) {
			x = a;
			y = b;
			value = F;
		}

	};



	struct mpossibility {//possibility of action
		action act;
		double prob;
		mpossibility(action a, double p) {
			act = a;
			prob = p;
		}
		mpossibility() {}

		void update(double inc) {
			prob += inc;
		}
	};

	struct mazeMoves {//4 possible moves from each state
		mpossibility acts[4];
		mazeMoves(double u, double d, double l, double r) {

			mpossibility* posPtr = new mpossibility(Up, u);
			acts[0] = *posPtr;
			posPtr = new mpossibility(Down, d);
			acts[1] = *posPtr;
			posPtr = new mpossibility(Left, l);
			acts[2] = *posPtr;
			posPtr = new mpossibility(Right, r);
			acts[3] = *posPtr;
		}
		boolean isPossible(action act) {
			return (acts[act].prob != -10000);
		}
		mazeMoves() {}//default constructor
		
	

		void print() {// print porobabilities for state
			cout << acts[0].prob << " " << acts[1].prob << " " << acts[2].prob <<
				" " << acts[3].prob << " * ";
		}
	};

	
	mazeMoves** policy;//policy matrix
	mazeMoves** actionValues;//action-value matrix we can use same structures both for policy & actionValues,
						 //the only diffrence is that for actionValues probs show values, nit actual probability
	int size;// size of square maze
	cells** maze;//maze map
	action buf; //buffer
	double learningRate;
	double discountingFactor;
	moveType currentMoveType = Greedy;
	double epsilon = 0;
	const int actionsNum = 4;
	state* pointerToState = new state(0, 0);
	state currentState = *pointerToState;
	double temperature;


	void defaultActionValues() {//default action-values evaluation, possible by zero impossible by -10000
		actionValues = new mazeMoves*[size];
		
		double up, down, left, right;
		for (int i = 0; i < size; i++) {
			actionValues[i] = new mazeMoves[size];

			if (i == 0) {//first row - can't go up
				up = -10000;
			}
			else {
				up = 0;
			}
			if (i == size - 1) {//last row - can't go down
				down = -10000;
			}
			else {
				down = 0;
			}

			for (int j = 0; j < size; j++) {//actions - up, down, left, right					
				if (j == 0) {//first column - can't go left
					left = -10000;
				}
				else {
					left = 0;
				}
				if (j == size - 1) {//last column - can't go right
					right = -10000;
				}
				else {
					right = 0;
				}
				mazeMoves* move = new mazeMoves(up, down, left, right);
				actionValues[i][j] = *move;

			}
		}
	}


	void defaultPolicy() {//default policy - from each state all possible actions are uniformly distributed 
		
		policy = new mazeMoves*[size];
		
		for (int i = 0; i < size; i++) {
			double up, down;
			int avail = 4;
			policy[i] = new mazeMoves[size];
			if (i == 0) {//first row - can't go up
				up = 0;
				avail--;
			}
			else up = 1;
			if (i == size - 1) {//last row - can't go down
				down = 0;
				avail--;
			}
			else down = 1;


			for (int j = 0; j < size; j++) {//actions - up, down, left, right		
				int thisAvail = avail; //not to affect rows' avail
				double left, right;
				if (j == 0) {//first column - can't go left
					left = 0;
					thisAvail--;
				}
				else left = 1;

				if (j == size - 1) {//last column - can't go right
					right = 0;
					thisAvail--;
				}
				else right = 1;
				if (thisAvail != 0) {
					up = up / thisAvail;
					down = down / thisAvail;
					left = left / thisAvail;
					right = right / thisAvail;
				}
				mazeMoves* mov = new mazeMoves(up, down, left, right);
				policy[i][j] = *mov;

			}
		}
	}

public:

	int avgSteps=0;//avg steps per episode (last more valuable)
	int runs = 0;//episodes run


	explicit mazeAgent(int sz, double LR, double DF, int mt, int **mazeMap) {
		learningRate = LR;
		discountingFactor = DF;
		size = sz;
		currentMoveType = (moveType)mt;
		maze = new cells*[size];
		for (int i = 0; i < size; i++) {
			maze[i] = new cells[size];
			for (int j = 0; j < size; j++) {
				maze[i][j] = (cells)mazeMap[i][j];
			}
		}
		defaultActionValues();
		defaultPolicy();
	}


	double gamble() {//random number from 0 to 1000
		srand(rand());
		return static_cast<double>(rand() % 1000);
	}

	void setEpsilon(double eps) {//for e-greedy
		epsilon = eps;
	}

	void setTemperature(double tmp) {//for softmax
		temperature = tmp;
	}

	void changePolicy(moveType mt) {//change traversal type
		currentMoveType = mt;
	}


	int reward(cells cell){//count reward
		if (cell == G) {
			return 10;
		}
		else if (cell == H) { return -10; }
		else return -1;
	}

	int getSize() {//getter
		return size;
	}

	

	void showMovesMatr(mazeMoves** matr) {//print policy or action-values matrix
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size-1; j++) {
				matr[i][j].print();
			}
			matr[i][size-1].print();
			cout << endl;
		}
	}
	void showValues() {
		showMovesMatr(actionValues);
	}
	
	string strMovesMatr(mazeMoves** matr) {//same for file ouput
		string result = "";
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size - 1; j++) {
				for (int k = 0; k < 4; k++) {
					result += ""+(int)matr[i][j].acts[k].prob;
					result += " ";
				}
			}
			matr[i][size - 1].print();
			for (int k = 0; k < 4; k++) {
				result += " "+((int)policy[i][size-1].acts[k].prob);
				result += " ";
			}
			result += "\n";
		}
		return result;
	}

	string printValues() {
		return strMovesMatr(actionValues);
	}
	
	int move() {//make move according to current moveType
		if (currentMoveType == Greedy) {
			return greedyMove();
		}
		else if (currentMoveType == epsilonGreedy) {
			return epsilonGreedyMove(epsilon);
		}

		return softMax();

	}
	int moves = 0;

	boolean makeAction(action act) {//change state

		if ((act == Up)&&(currentState.x>0)) {//checking if move is possible
			currentState.x--;
		}
		else if ((act == Down)&&(currentState.x<size-1)) {
			currentState.x++;
		}
		else if ((act == Left)&&(currentState.y>0)) {
			currentState.y--;
		}
		else if ((act == Right)&&(currentState.y<size-1)) {
			currentState.y++;
		}
		else { return false; }//if not - say you can't 
		return true;
	}

	int greedyMove() {

		int x = currentState.x;
		int y = currentState.y;
		mazeMoves current = policy[x][y];//take entity from policies for current state

		state old = currentState;//for checking that move succeed 
		
		double max = actionValues[currentState.x][currentState.y].acts[0].prob;//choose highest valued action, initially - zeroes
		int m = 0;
		int n = 2;//for distribution close to uniform
		for (int i = 1; i < 4; i++) {
			if (actionValues[currentState.x][currentState.y].acts[i].prob > max) {
				max = actionValues[currentState.x][currentState.y].acts[i].prob;
				m = i;
			}
			else if (actionValues[currentState.x][currentState.y].acts[i].prob == max) {
				//to make random choise in case of multiple actions with the same value
				double c = gamble();
				m = (c<(1000/n)) ? i : m;
				n++;
			}

		}
		//put action in buffer and do it
		if (makeAction(current.acts[m].act)) {
			buf = current.acts[m].act;
		}
		else {
			moves++;
			if (moves < 5) {
				return move();
			}
			else {
				makeAction(op(current.acts[m].act));
				buf = op(current.acts[m].act);
				moves = 0;
			}
		}
	
		return reward(maze[currentState.x][currentState.y]);//return reward

	}

	int epsilonGreedyMove(double epsilon) {

		int x = currentState.x;
		int y = currentState.y;
		mazeMoves next = policy[x][y];

		if (gamble() / 1000 > epsilon) {//do greedy move
			return greedyMove();
		}
		
		else {//or do random move according to it's possibility
			double chance = gamble()/1000;
			for (int i = 0; i < 4; i++) {
				
				state old = currentState;
				if (next.acts[i].prob > chance) {//probability of action (in policy) higher htan dice
					
					if (makeAction(next.acts[i].act)) {
						buf = next.acts[i].act;
					}
					else {
						throw exception("Error!!!");
					}
					break;
				}
				else {
					chance -= next.acts[i].prob;
				}
			}
			return reward(maze[currentState.x][currentState.y]);
		}
	}

	int softMax()  {
		double* chanses = new double[actionsNum];
		double denominator = 0;
		double check = 0;
		for (int i = 0; i < actionsNum; i++) {//choose probabilities by softmax formula
			denominator += exp((actionValues[currentState.x][currentState.y].acts[i].prob) / temperature);
		}
		for (int i = 0; i < actionsNum; i++) {
			chanses[i] = exp((actionValues[currentState.x][currentState.y].acts[i].prob) / temperature) / denominator;
			check += chanses[i];
		}
		int x = currentState.x;
		int y = currentState.y;
		mazeMoves next = policy[x][y];
		double dice = gamble()/1000;
		for (int i = 0; i < actionsNum; i++) {//if gamble < chance - move; else check next variant
			if (chanses[i] >= dice) {
				
				if (makeAction(next.acts[i].act)) {
					buf = next.acts[i].act;
				}
				else {
					moves++;
					if (moves < 5) {
						return move();
					}
					else {
						makeAction(op(next.acts[i].act));
						buf = op(next.acts[i].act);
						moves = 0;
					}
				}
				break;
			}
			else {
				dice -= chanses[i];
			}
		}
		return reward(maze[currentState.x][currentState.y]);
	}

	int steps = 0;

};



