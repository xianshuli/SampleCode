class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> result;
        vector<int> positionForRow;//initialize to be empty
        helper(n, 0, positionForRow, result);
        return result;
    }
private:
    void helper(int n, int row, vector<int>& positionForRow, vector<vector<string>>& result) {
        if (row == n) {//construct the solution
            vector<string> solution(n, string(n, '.'));
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (j == positionForRow[i]) {
                        solution[i][j] = 'Q';
                    }
                }
            }
            result.push_back(solution);
			return;//never forget to add "return"!!!
        }
        for (int i = 0; i < n; ++i) {
            positionForRow.push_back(i);
            //this is diffrent from code ganker, I still try to follow the common sense of DFS(first push_back, then pop_back, so the positionForRow is initialized to be empty)
            if (isValid(row, positionForRow)) {
                helper(n, row + 1, positionForRow, result);
            }
            positionForRow.pop_back();
        }
    }
    bool isValid(int row, vector<int>& positionForRow) {
        for (int i = 0; i < row; ++i) {
            if (positionForRow[i] == positionForRow[row]
            || abs(positionForRow[i] - positionForRow[row]) == row - i) {
                return false;
            }
        }
        return true;
    }
};
