all: build
build: echo_s echo_c log_s
echo_s: echo_s.c
	g++ -Wall -g -o echo_s echo_s.c
echo_c: echo_c.c
	g++ -Wall -g -o echo_c echo_c.c
log_s: log_s.c
	g++ -Wall -g -o log_s log_s.c
run_echo_s:
	./echo_s 55000
run_echo_c:
	./echo_c 55000
run_log_s:
	./log_s
clean:
	rm -f echo_s echo_c log_s
