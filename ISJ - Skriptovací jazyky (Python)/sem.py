def f(a = 1, b = 1, u = 40000):
	while a+b < u:
		yield b
		a,b = b, a+b
 
print(list(range(3)))

gen = f(u = 30)
for i in gen:
	print(i)

print(sum(k**3 +g for k in range(3) for g in f(u = 30) if g%2 and k!=g))
_sum = 0
for g in f(u=30):
	for k in range(3):
		if g%2 and k!=g:
			_sum += k**3+g
print(_sum)