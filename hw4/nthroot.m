function [ output_args ] = nthroot( x, n, initial, numiterations )

X = zeros(1,numiterations+1);
X(1) = initial;
for i=2:numiterations+1
    
    X(i) = 1/n * ( (n-1)*X(i-1) + x / (X(i-1))^(n-1));

end

output_args = X;

end