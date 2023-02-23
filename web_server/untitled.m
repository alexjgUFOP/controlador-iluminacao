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

figure
hold on
i = 1;
while true
    read = webread('http://192.168.2.201:8080/fluxo');
    data = str2num(read);
    x(i) = data(1);
    y(i) = i;
    plot(y,x)
    i = i + 1;
end