
int a = 2;
int b = 3;
int c = 0;
int i = 0;


while (i <= 5) {
    c = c + i;
    i = i + 1;
}


int d = a * b;
int e = c - d;


if (e > 0) {
    e = e + 10;
} else {
    e = e - 10;
}


return e;
