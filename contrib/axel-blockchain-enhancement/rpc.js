const axios = require("axios")
const URL = require("url-parse")

class RPC {
    constructor(_baseURL) {
        this._baseURL = _baseURL;
        this.idNonce = 0;
        const url = new URL(_baseURL);
        const config = {
            baseURL: url.origin,
            // don't throw on non-200 response
            validateStatus: () => true,
        };
        if (url.username !== "" && url.password !== "") {
            config.auth = {
                username: url.username,
                password: url.password,
            };
        }
        this._api = axios.default.create(config);
    }

    async rawCall(method, params = [], opts = {}) {
        const rpcCall = {
            method,
            params,
            id: this.idNonce++,
        };
        let res = await this.makeRPCCall(rpcCall);
        if (res.status === 402) {
            const auth = res.data;
            res = await this.authCall(auth.id, rpcCall);
        }
        if (res.status === 401) {
            throw new Error(await res.statusText);  // empty body
        }
        if (res.status === 404) {
            throw new Error(`unknown method: ${method}`);
        }
        if (res.status !== 200) {
            if (res.headers["content-type"] !== "application/json") {
                const body = await res.data;
                throw new Error(`${res.status} ${res.statusText}\n${body}`);
            }
            const eresult = await res.data;
            if (eresult.error) {
                const { code, message, } = eresult.error;
                throw new Error(`[${code}] ${message}`);
            }
            else {
                throw new Error(String(eresult));
            }
        }
        const { result } = await res.data;
        return result;
    }

    makeRPCCall(rpcCall) {
        return this._api.post("/", rpcCall);
    }
}

exports.RPC = RPC;
