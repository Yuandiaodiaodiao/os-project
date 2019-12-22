import multiprocessing
import os
import signal


def produce(id):
    os.system("/tmp/tmp.FPB8OXWmxk/cmake-build-debug/project3 " + str(id))


def smoker(id):
    os.system("/tmp/tmp.dr3A3550ut/cmake-build-debug/project31 " + str(id))


def sigint_handler(signum, frame):
    global p
    for i in p:
        i.terminate()
    global s
    for i in s:
        i.terminate()
    print("end")


if __name__ == "__main__":
    s = []
    for i in range(3):
        s.append(multiprocessing.Process(target=smoker, args=(str(i + 1),)))
        s[i].start()
    signal.signal(signal.SIGINT, sigint_handler)
    p = []
    for i in range(2):
        p.append(multiprocessing.Process(target=produce, args=(str(i + 1),)))
        p[i].start()


