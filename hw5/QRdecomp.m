function [ T ] = QRdecomp( A )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

dimensions = size(A);
n = dimensions(1);

R = zeros(n,n);
Q = zeros(n,n);
Q1 = zeros(n,n);

Q1(:,1) = A(:,1);
R(1,1) = norm(Q1(:,1));
Q(:,1) = Q1(:,1) / R(1,1);

for k=1:n-1
    w = A(:,k+1);
    for i=1:k
        R(i,k+1) = dot(A(:,k+1),Q(:,i));
        w = w - R(i,k+1) * Q(:,i);
    end
    Q1(:,k+1) = w;
    R(k+1,k+1) = norm(Q1(:,k+1));
    Q(:,k+1) = Q1(:,k+1) / R(k+1,k+1);
end

T = [Q,R];

end