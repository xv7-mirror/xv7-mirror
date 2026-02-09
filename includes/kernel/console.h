#define INPUT_BUF 128
struct input_struct {
    char buf[INPUT_BUF];
    uint r; // Read index
    uint w; // Write index
    uint e; // Edit index
    struct proc* fgproc;
};

extern struct input_struct input;
