float fun(float var2) {
	float var2;
	var2 += 9;
	return var2;
}

ret = fun(3);

if (ret == 12) exit("function 'fun' ignored local var decl", -1);
