// This a class that's designed to help us measure how objects
// are used in certain containers and algorithms. Mainly records
// how often certain common methods (construction, comparison)
// are invoked.
class NoisyClass {
    public:
    static int COPY_COUNTER;
    static int MOVE_COUNTER;
    static int CONSTRUCTION_COUNTER;
    static int LT_COUNTER;
    int x;
    NoisyClass(const NoisyClass& n): x(n.x) { /*printf("copy!\n");*/ COPY_COUNTER++; }
    NoisyClass(NoisyClass&& n): x(n.x) { /*printf("move!\n");*/ MOVE_COUNTER++; }
    NoisyClass() = delete;
    explicit NoisyClass(int x): x(x) { /*printf("construction!\n");*/ CONSTRUCTION_COUNTER++; }
    bool operator==(const NoisyClass& n) const { return x == n.x; }
    bool operator>=(const NoisyClass& n) const { return x >= n.x; }
    bool operator>(const NoisyClass& n) const { return x > n.x; }
    bool operator<(const NoisyClass& n) const { LT_COUNTER++; return x < n.x; }
    operator int() { return x; }

    static void reset_state() {
        MOVE_COUNTER = 0;
        COPY_COUNTER = 0;
        CONSTRUCTION_COUNTER = 0;
        LT_COUNTER = 0;
    }
};

