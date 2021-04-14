
#docker build --tag=ofxget .
#docker run --rm -i ofxget -institution 479 -request bank.txt
#docker run --rm -i --volume $PWD/passwords.txt:/home/passwords.txt ofxget -institution 479 -request bank.txt

FROM gcc:latest as builder

WORKDIR /home

COPY . .

RUN make

#using this because its a parent of gcc:latest
FROM buildpack-deps:buster-curl

WORKDIR /home

COPY --from=builder /home/ofxget /home/ofxhome /home/ofxhome_test ./
COPY requests requests
COPY institutions.txt .

ENTRYPOINT ["/home/ofxget"]

