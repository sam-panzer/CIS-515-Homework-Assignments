function [ X ] = sequence( A )

k = 50;
dimensions = size(A);
n = dimensions(1);
A1 = A;

for i=1:k
    T = QRdecomp(A1);
    Q = T(:,1:n);
    R = T(:,n+1:2*n);
    A1 = R * Q;
    
end

X = A1;

end

