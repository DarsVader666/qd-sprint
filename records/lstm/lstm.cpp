#include <bits/stdc++.h>
using namespace std;

static inline double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }

int main() {
    int T, B, D, H;
    if (scanf("%d %d %d %d", &T, &B, &D, &H) != 4) return 0;

    vector<double> X((size_t)T * B * D);
    for (auto &v : X) scanf("%lf", &v);

    // gate order in input: input, forget, candidate, output
    vector<vector<double>> Wx(4), Wh(4), b(4);
    for (int q = 0; q < 4; ++q) {
        Wx[q].resize((size_t)D * H);
        Wh[q].resize((size_t)H * H);
        b[q].resize(H);
        for (auto &v : Wx[q]) scanf("%lf", &v);
        for (auto &v : Wh[q]) scanf("%lf", &v);
        for (auto &v : b[q]) scanf("%lf", &v);
    }

    vector<double> h((size_t)B * H, 0.0), c((size_t)B * H, 0.0);
    vector<double> pre((size_t)4 * B * H, 0.0);
    vector<double> out;
    out.reserve((size_t)(T + 1) * B * H);

    const size_t BH = (size_t)B * H;

    for (int t = 0; t < T; ++t) {
        fill(pre.begin(), pre.end(), 0.0);
        for (int bi = 0; bi < B; ++bi) {
            const double *xb = &X[((size_t)t * B + bi) * D];
            const double *hb = &h[(size_t)bi * H];
            double *p = &pre[(size_t)bi * H];
            for (int j = 0; j < H; ++j) {
                double s0 = 0, s1 = 0, s2 = 0, s3 = 0;
                for (int d = 0; d < D; ++d) {
                    double xv = xb[d];
                    s0 += xv * Wx[0][(size_t)d * H + j];
                    s1 += xv * Wx[1][(size_t)d * H + j];
                    s2 += xv * Wx[2][(size_t)d * H + j];
                    s3 += xv * Wx[3][(size_t)d * H + j];
                }
                for (int k = 0; k < H; ++k) {
                    double hv = hb[k];
                    s0 += hv * Wh[0][(size_t)k * H + j];
                    s1 += hv * Wh[1][(size_t)k * H + j];
                    s2 += hv * Wh[2][(size_t)k * H + j];
                    s3 += hv * Wh[3][(size_t)k * H + j];
                }
                p[j + 0 * BH] = s0 + b[0][j];
                p[j + 1 * BH] = s1 + b[1][j];
                p[j + 2 * BH] = s2 + b[2][j];
                p[j + 3 * BH] = s3 + b[3][j];
            }
        }
        for (size_t i = 0; i < BH; ++i) {
            double ft = sigmoid(pre[i + 1 * BH]);
            double it = sigmoid(pre[i + 0 * BH]);
            double ot = sigmoid(pre[i + 3 * BH]);
            double ct = tanh(pre[i + 2 * BH]);
            c[i] = ft * c[i] + it * ct;
            h[i] = ot * tanh(c[i]);
        }
        out.insert(out.end(), h.begin(), h.end());
    }
    out.insert(out.end(), c.begin(), c.end());

    for (size_t i = 0; i < out.size(); ++i) {
        if (i) putchar(' ');
        printf("%.4f", out[i]);
    }
    putchar('\n');
    return 0;
}
