BEGIN {
	n=0;
	t0=0;
	d=30;
}


{
	ct=t0+(n-1)*d
	#ct=50
	x=$1

	if(n != 0)
		printf("%s T%d ", x, ct);
	else
		printf("%s ", x);

	if(n % 5 == 0 && n != 0)
		printf("\n");
		
	n++;
}
