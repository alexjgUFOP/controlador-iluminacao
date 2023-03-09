for m = 1:100
    
    read(m,:) = webread('http://192.168.2.201:8080/fluxo')
end


x = 1:0.01:25;
y = sin(x);
n = numel(x);
figure;
for i = 1:n
    read = webread('http://192.168.2.201:8080/fluxo');
    data = str2num(read);
    x = data(1);
    y = data(2);
    addpoints(an,x(i),y(i));
    drawnow;
end


x = 1:0.01:25;
y = sin(x);
n = numel(x);

clear
clc
figure
hold on
i = 1;
while true
    read  = webread('http://192.168.2.201:8080/fluxo');
    data  = str2num(read);
    f(i)  = data(1);
    dc(i) = data(2);
    t(i)  = data(3)/1000;
    plot(t,f)
    %pause(0.10);
    i = i + 1;
end


L = 10.0358 - 10
T = 10.1000 - 10
K = 1345

g = tf([K*exp(-L)],[T 1])


