#include <iostream>
using namespace std;
void multiply(int x, int y) {
    int ans = x*y;
    cout << ans << endl;
}
void divide(int x, int y) {
    int ans = x/y;
    cout << ans << endl;
}
void add(int x, int y) {
    int ans = x+y;
    cout << ans << endl;
}
void subtract(int x, int y) {
    int ans = x-y;
    cout << ans << endl;
}
int main() {
    int x; int y; int z;
    cin >> z; cin >> x; cin >> y;
    switch (z) {
        case 1:
        multiply(x,y);
        break;
        case 2:
        divide(x,y);
        break;
        case 3:
        add(x,y);
        break;
        case 4:
        subtract(x,y);
        break;
    }
    return 0;
}