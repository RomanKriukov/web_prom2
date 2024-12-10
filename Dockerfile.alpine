FROM python:alpine

WORKDIR /app

EXPOSE 5000

ENV TZ=Europe/Kyiv

# dependencies for pyodbc
RUN apk update \
    && apk add --no-cache \
        gcc \
        g++ \
        unixodbc \
        unixodbc-dev \
    && rm -rf /var/cache/apk/*

# Install OpenSSL
RUN apk update --no-cache && apk upgrade --no-cache openssl  

# Add openssl dependencies for wkhtmltopdf
RUN echo 'http://dl-cdn.alpinelinux.org/alpine/v3.8/main' >> /etc/apk/repositories && \
    apk add --no-cache libcrypto1.0 libssl1.0

# Install dependencies
RUN apk --no-cache add curl gnupg

# Download the desired package(s)
RUN curl -O https://download.microsoft.com/download/e/4/e/e4e67866-dffd-428c-aac7-8d28ddafb39b/msodbcsql17_17.6.1.1-1_amd64.apk
RUN curl -O https://download.microsoft.com/download/e/4/e/e4e67866-dffd-428c-aac7-8d28ddafb39b/mssql-tools_17.6.1.1-1_amd64.apk

# # (Optional) Verify signature, if 'gpg' is missing install it using 'apk add gnupg':
# RUN curl -O https://download.microsoft.com/download/e/4/e/e4e67866-dffd-428c-aac7-8d28ddafb39b/msodbcsql17_17.6.1.1-1_amd64.sig
# RUN curl -O https://download.microsoft.com/download/e/4/e/e4e67866-dffd-428c-aac7-8d28ddafb39b/mssql-tools_17.6.1.1-1_amd64.sig

# RUN curl https://packages.microsoft.com/keys/microsoft.asc  | gpg --import -
# RUN gpg --verify msodbcsql17_17.6.1.1-1_amd64.sig msodbcsql17_17.6.1.1-1_amd64.apk
# RUN gpg --verify mssql-tools_17.6.1.1-1_amd64.sig mssql-tools_17.6.1.1-1_amd64.apk

# Install the package(s)
RUN apk add --allow-untrusted msodbcsql17_17.6.1.1-1_amd64.apk
RUN apk add --allow-untrusted mssql-tools_17.6.1.1-1_amd64.apk

RUN pip install pipenv

COPY Pipfile* venv /app/

# RUN pipenv install

RUN pip install flask
RUN pip install sqlalchemy
RUN pip install pyodbc
RUN pip install pandas
RUN pip install numpy
RUN pip install flask-wtf
RUN pip install flask-cors
RUN pip install pyjwt

COPY . .

CMD ["pipenv", "run", "python", "runserver.py"]
