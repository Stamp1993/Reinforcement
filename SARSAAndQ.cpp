#include"agent.h"
#include<fstream>

class sarsaAgent: public mazeAgent {

public:

	using mazeAgent::mazeAgent;
	int learnedOn = 0;//how much steeps was needed to learn

	double evaluate() {

		state last = currentState;//save state was current on the enering evaluation function
		int rew = move();//move and save reward
		action done = buf; // save last action done
		cells alloc = maze[currentState.x][currentState.y];//save value of cell we came to
		mpossibility actDone = actionValues[last.x][last.y].acts[done];//last action done
		double Qsa = actDone.prob;//action-value of last action
		int mistakes = 0;//number of mistakes


		if (alloc == H || alloc == G || steps == 2000) {//if finishing cell or wandering too long
			string c = (buf == Left) ? "left" : ((buf == Right) ? "Right" : ((buf == Up) ? "Up" : "Down"));
			cout << "step " << steps << " dir " << c << " reward " << rew << endl;
			runs++;
			if ((learnedOn == 0) && rew == 10) {//if came to goal - we possibly learned
				learnedOn = runs;
				Sleep(1000);
			}
			else if (rew == 10) {
				mistakes = 0;
			}
			else if (learnedOn != 0 && rew != 10) {//if we didn't come three times in a row - we possibly didn't learned
				if (mistakes >= 3) {
					learnedOn = 0;
					mistakes = 0;
				}
				else {
					mistakes++;
				}
			}
			avgSteps = (avgSteps + steps) / 2;
			currentState.x = 0;
			currentState.y = 0;
			steps = 0;
			cout << "avg" << avgSteps << endl;

			cout << "learned " << learnedOn << endl;

			actionValues[last.x][last.y].acts[done].update(learningRate*(rew - Qsa));//update Q and go to start


		}
		else {
			steps++;
			//learn
			actionValues[currentState.x][currentState.y].acts[done].update(learningRate*(rew + discountingFactor*evaluate() - Qsa));

		}

		double Q = actionValues[last.x][last.y].acts[done].prob;

		return Q;


	}

};

class QAgent : public mazeAgent {

public:
	using mazeAgent::mazeAgent;
	int learnedOn = 0;

	double evaluate() {

		state last = currentState;
		int rew = move();
		action done = buf;
		cells alloc = maze[currentState.x][currentState.y];
		mpossibility actDone = actionValues[last.x][last.y].acts[done];
		double Qsa = actDone.prob;
		int mistakes=0;
		

		if (alloc == H || alloc == G||steps==2000) {//if finishing cell or wandering too long
			string c = (buf == Left) ? "left" : ((buf == Right) ? "Right" : ((buf == Up) ? "Up" : "Down"));
			cout << "step " << steps << " dir " << c << " reward " << rew << endl;
			runs++;
			if ((learnedOn == 0) &&rew==10) {
				learnedOn = runs;
				Sleep(1000);
			}
			else if (rew == 10) {
				mistakes = 0;
			}
			else if (learnedOn != 0 && rew != 10) {
				if (mistakes >= 3) {
					learnedOn = 0;
					mistakes = 0;
				}
				else {
					mistakes++;
				}
			}
			avgSteps = (avgSteps + steps) / 2;
			currentState.x = 0;
			currentState.y = 0;
			steps = 0;
			cout << "avg" << avgSteps << endl;//update Q and go to start
										
			cout << "learned " << learnedOn << endl;

			actionValues[last.x][last.y].acts[done].update( learningRate*(rew - Qsa));
			

		}
		else {
			steps++;
			//qlearn
			actionValues[currentState.x][currentState.y].acts[done].update( learningRate*(rew + discountingFactor*evaluate() - Qsa));
		
		}

		mazeMoves Q = actionValues[last.x][last.y];

		double max = Q.acts[0].prob;
		for (int i = 1; i < 4; i++) {//find max action value
			max = (max > Q.acts[i].prob) ? max : Q.acts[i].prob;
		}
		return max;


	}

	


};

int main() {

	int** maze4;
	maze4 = new int*[4];
	for (int i = 0; i < 4; i++) {
		maze4[i] = new int[4];
	}
	
	int arr0[4] = { 0, 1, 1, 1 };
		maze4[0] = arr0;
		int arr1[4] = { 1, 2, 1, 2 };
		maze4[1] = arr1;
		int arr2[4] = { 1, 1, 1, 2 };
		maze4[2] = arr2;
		int arr3[4] = { 2, 1, 1, 3 };
		maze4[3] = arr3;
	
	
	int** maze8;
	maze8 = new int*[8];
	for (int i = 0; i < 8; i++) {
		maze8[i] = new int[8];
	}
	
	int arr80[8] = { 0,1,1,1,1,1,1,1 };
	maze8[0] = arr80;
	int arr81[8] = { 1,1,1,1,1,1,1,1 };
	maze8[1] = arr81;
	int arr82[8] = { 1,1,1,2,1,1,1,1 };
	maze8[2] = arr82;
	int arr83[8] = { 1,1,1,1,1,2,1,1 };
	maze8[3] = arr83;
	int arr84[8] = { 1,1,1,2,1,1,1,1 };
	maze8[4] = arr84;
	int arr85[8] = { 1,2,2,1,1,1,2,1 };
	maze8[5] = arr85;
	int arr86[8] = { 1,2,1,1,2,1,2,1 };
	maze8[6] = arr86;
	int arr87[8] = { 1,1,1,2,1,1,1,3 };
	maze8[7] = arr87;
	


	QAgent qagent1 = QAgent(4, 0.1, 0.9, 0, maze4);//greedy 4x4

	QAgent qagent2 = QAgent(8, 0.1, 0.9, 0, maze8);//greedy 8x8
	sarsaAgent sagent1 = sarsaAgent(4, 0.1, 0.9, 0, maze4);
	
	sarsaAgent sagent2 = sarsaAgent(8, 0.1, 0.9, 0, maze8);
	QAgent qagent3 = QAgent(8, 0.1, 0.9, 1, maze8);//e-greedy 4x4
	qagent3.setEpsilon(0.05);
	QAgent qagent4 = QAgent(8, 0.1, 0.9, 2, maze8);//softmax 4x4
	qagent4.setTemperature(0.01);
	sarsaAgent sagent3 =sarsaAgent(8, 0.1, 0.9, 1, maze8);
	sagent3.setEpsilon(0.05);
	sarsaAgent sagent4 = sarsaAgent(8, 0.1, 0.9, 2, maze8);
	sagent4.setTemperature(0.02);

	srand(rand()+7);
	srand(rand()-1);


	for (int i = 0; i < 1001; i++) {
		qagent1.evaluate();
		qagent2.evaluate();
		qagent3.evaluate();
		qagent4.evaluate();

		sagent1.evaluate();
		sagent2.evaluate();
		sagent3.evaluate();
		sagent4.evaluate();
		if (i % 100 == 0) {
			qagent1.showValues();
			qagent2.showValues();
			qagent3.showValues();
			qagent4.showValues();
			sagent1.showValues();
			sagent2.showValues();
			sagent3.showValues();
			sagent4.showValues();
		}
	}
	cout << "learned on " << qagent1.learnedOn << endl;
	cout << "learned on " << qagent2.learnedOn << endl;
	cout << "learned on " << qagent3.learnedOn << endl;
	cout << "learned on " << qagent4.learnedOn << endl;
	cout << "learned on " << sagent1.learnedOn << endl;
	cout << "learned on " << sagent2.learnedOn << endl;
	cout << "learned on " << sagent3.learnedOn << endl;
	cout << "learned on " << sagent4.learnedOn << endl;

	
	system("pause");
}