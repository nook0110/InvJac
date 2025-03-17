import sympy as sp
import itertools

x, y = sp.symbols('x y')

monomials = [x**i * y**j for i in range(4) for j in range(4) if 2 <= i + j <= 3]

polynomials = []
for coeffs in itertools.product([0, 1], repeat=len(monomials)):
    polynomial = sum(c * m for c, m in zip(coeffs, monomials))
    polynomials.append(polynomial)

for i, poly1 in enumerate(polynomials):
    for j, poly2 in enumerate(polynomials):
            print(2)
            print(str(x + poly1).replace('**', '^'))
            print(str(y + poly2).replace('**', '^'))
