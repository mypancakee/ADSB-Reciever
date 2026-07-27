class Debug{
    static RESET = "\x1b[0m";

    static YELLOW = "\x1b[33m";
    static GREEN = "\x1b[32m";
    static RED = "\x1b[31m";
    static CYAN = "\x1b[36m";


    static currentTime(){
        return new Date().toLocaleTimeString(
            "en-GB",
            {
                hour12: false
            }
        );
    }


    static log(className, message){
        console.log(
            `${this.YELLOW}[${this.currentTime()}] [DEBUG] ${className}: ${message}${this.RESET}`
        );
    }


    static success(className, message){
        console.log(
            `${this.GREEN}[${this.currentTime()}] [SUCCESS] ${className}: ${message}${this.RESET}`
        );
    }


    static error(className, message){
        console.error(
            `${this.RED}[${this.currentTime()}] [ERROR] ${className}: ${message}${this.RESET}`
        );
    }


    static info(className, message){
        console.log(
            `${this.CYAN}[${this.currentTime()}] [INFO] ${className}: ${message}${this.RESET}`
        );
    }
}


export default Debug;