# bezüglich tzdata siehe:
# https://stackoverflow.com/questions/44331836/apt-get-install-tzdata-noninteractive und
# https://askubuntu.com/questions/909277/avoiding-user-interaction-with-tzdata-when-installing-certbot-in-a-docker-contai
# Setzen TZ:
# https://linuxconfig.org/how-to-change-timezone-on-ubuntu-18-04-bionic-beaver-linux
FROM ubuntu:bionic
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata && \
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
