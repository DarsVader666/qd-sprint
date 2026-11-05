import sys, numpy as np

def main():
    data = sys.stdin.read().split()
    it = iter(data)
    T, B, D, H = (int(next(it)) for _ in range(4))
    X = np.array([float(next(it)) for _ in range(T*B*D)]).reshape(T, B, D)
    gates = []
    for _ in range(4):
        Wx = np.array([float(next(it)) for _ in range(D*H)]).reshape(D, H)
        Wh = np.array([float(next(it)) for _ in range(H*H)]).reshape(H, H)
        b = np.array([float(next(it)) for _ in range(H)])
        gates.append((Wx, Wh, b))
    Wi, Wf, Wc, Wo = gates[0][0], gates[1][0], gates[2][0], gates[3][0]
    Whi, Whf, Whc, Who = gates[0][1], gates[1][1], gates[2][1], gates[3][1]
    bi, bf, bc, bo = gates[0][2], gates[1][2], gates[2][2], gates[3][2]
    sig = lambda x: 1/(1+np.exp(-x))
    h = np.zeros((B, H)); c = np.zeros((B, H)); hs = []
    for t in range(T):
        xt = X[t]
        ft = sig(xt@Wf + h@Whf + bf)
        it_ = sig(xt@Wi + h@Whi + bi)
        ct_ = np.tanh(xt@Wc + h@Whc + bc)
        c = ft*c + it_*ct_
        ot = sig(xt@Wo + h@Who + bo)
        h = ot*np.tanh(c)
        hs.append(h.copy())
    out = np.concatenate(hs).flatten().tolist() + c.flatten().tolist()
    print(' '.join(f'{v:.4f}' for v in out))

main()
