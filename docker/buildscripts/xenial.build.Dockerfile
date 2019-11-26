FROM ubuntu:xenial
RUN apt-get update && \
    apt-get install -y python3 \
      python3-pip \
      git \
      python3-dev \
      tk-dev \
      avr-libc \
      swig \
      iverilog \
      make \
      cmake \
      g++ \
      valgrind \
      nano-tiny \
      help2man \
      texinfo \
      time \
      doxygen \
      graphviz && \
    rm -rf /var/lib/apt/lists/* && \
    cd /root && \
    git clone https://github.com/rst2pdf/rst2pdf.git && \
    cd rst2pdf && \
    pip3 install . && \
    pip3 install sphinx beautifulsoup4 && \
    useradd -m buildbudy
ENTRYPOINT ["/bin/bash"]
CMD []
