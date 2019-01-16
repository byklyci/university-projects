pragma solidity ^0.5.0;

/// @title An autonomous decentralized lottery as a Solidity smart contract
/// @author Baran Kılıç
contract AutonomousLottery {
    
    // Stores information about ticket. This struct will be associated with a ticket number.
    struct TicketInfo {
        address purchaser; // address of ticket purchaser
        bytes32 commitment; // keccak256 hash of the (random number || purchase address)
                            // that the ticket purchaser provides
        bool revealed; // true if purchaser reveals the random number
        bool refunded; // true if purchaser refunds ticket
        bool prizeCollected; // true if purchaser withdraws the prize for ticket
    }
    
    // Stores information for one week of lottery.
    // The start and end dates of the lottery will be calculated from startOfLottery variable and
    // index of Lottery struct in the lotteries array.
    struct Lottery {
        uint collectedMoney; // total collected money for lottery during 
                             // purchase period in terms of ether
        mapping ( uint => TicketInfo) tickets; // ticket are represented with numbers from 0 to 99999
                                               // for the sake of simplicity there will be not leading
                                               // zeros in ticket numbers
                                               // e.g. 00000 will be represented as 0.
        uint[23] sumOfRandomNumbers; // sum of random numbers for each prize category
        uint indexForNextRandom; // the next random number provided by purchaser 
                                 // will be added to the prize category with this index
        bool donatedToCharity; // will be true if excess money is donated to the charity
    }
    
    Lottery[] public lotteries; // dynamic sized array to store lottery info
    
    uint public startOfLottery; // start timestamp of the first week of the lottery
    address payable public charity; // residual money after prize distribution will be transfered to this address
    uint constant SUM_OF_PRIZES = 68900;
    
    modifier hasTwoEthers(uint value) {
        require(
            2 ether == value,
            "Two ethers should be sent to purchase a ticket"
        ); _;
    }
    
    // only the tickets in the range 0...99999 will be sold
    modifier ticketInRange(uint ticket) {
        require(
            ticket < 100000,
            "The largest ticket number is 99999."
        ); _;
    }
    
    modifier ticketBelongsToMessageSender(uint week,uint ticket) {
        require(
            lotteries[week-1].tickets[ticket].purchaser == msg.sender, 
            "This ticket does not belong to you."
        ); _;
    }
    
    modifier randomNumberRevealed(uint week, uint ticket) {
        require(
            lotteries[week-1].tickets[ticket].revealed,
            "The random number is not revealed."
        ); _;
    }
    
    modifier prizeNotCollected(uint week, uint ticket) {
        require(
            !lotteries[week-1].tickets[ticket].prizeCollected,
            "The prize is already collected."
        ); _;
    }
    
    modifier lotteryEnded(uint week) {
        require(
            startOfLottery + week * 1 weeks < now,
            "This lottery has not yet been ended."
        ); _;
    }
    
    modifier lotteryExists(uint week) {
        require(
            lotteries.length >= week,
            "No ticket is purchased in this week of lottery."
        ); _;
    }
    
    constructor(address payable _charity) public {
        lotteries.push(Lottery({
            donatedToCharity: false,
            collectedMoney: 0,
            indexForNextRandom: 0,
            sumOfRandomNumbers: [uint(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        }));
        
        charity = _charity;
        startOfLottery = block.timestamp;
    }
    
    /// @notice Buys ticket for the purchaser for 2 ethers
    /// Format of random number hash `randomNumHash` = keccak256(abi.encodePacked(randomNum,msg.sender)).
    /// Alternatively, use the provided calculateRandomNumHash function to calculate hash
    /// @param ticket The number of ticket without leading zeros that is requested
    /// @param randomNumHash keccak256 hash of the (random number || purchase address)
    /// @return the week of lottery (one indexed e.g returns one for first lottery)
    function buyTicket(uint ticket, bytes32 randomNumHash) public payable
        hasTwoEthers(msg.value)
        ticketInRange(ticket) 
        returns(uint weekNumber)
    {
        // automatically creates new lottery if the last lottery ended until the lottery that
        // corresponds to current time is created
        while(startOfLottery + lotteries.length * 1 weeks < now) { 
            lotteries.push(Lottery({
                donatedToCharity: false,
                collectedMoney: 0,
                indexForNextRandom: 0,
                sumOfRandomNumbers: [uint(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            }));
        }
        
        // checks if we are in the purchase stage
        require(
            startOfLottery + (lotteries.length-1) * 1 weeks < now && 
            now <= startOfLottery + (lotteries.length-1) * 1 weeks + 5 days,
            "The lottery is not in purchase stage."
        );
        
        // check if requested ticket is available
        require(
            lotteries[lotteries.length-1].tickets[ticket].purchaser == address(0),
            "This ticket has already been purchased."
        );
    
        // saves the purchaser and hash info
        lotteries[lotteries.length-1].tickets[ticket].purchaser = msg.sender;
        lotteries[lotteries.length-1].tickets[ticket].commitment = randomNumHash;
        lotteries[lotteries.length-1].collectedMoney += 2;
        
        return lotteries.length; // week of lottery
    }
    
    /// @notice If enough money is collected, reveals the random number. Otherwise, refunds the money.
    /// Format of random number hash `randomNumHash` = keccak256(abi.encodePacked(randomNum,msg.sender)).
    /// Alternatively, use the provided calculateRandomNumHash function to calculate hash
    /// @param ticket The number of ticket without leading zeros that is requested
    /// @param randomNum The random number whose hash has been sent during purchase period
    /// @return Boolean pair for revealed and refunded info. If enough money is collected and the 
    /// provided randomNum is correct, it will return true,false. If randomNum is not correct, it will
    /// return false,false. If enough money is not collected, it will refund and will return false,true.
    function reveal(uint ticket, uint randomNum) public returns(bool revealed, bool refunded) {
        // automatically creates new lottery if the last lottery ended until the lottery that
        // corresponds to current time is created
        while(startOfLottery + lotteries.length * 1 weeks < now) {
            lotteries.push(Lottery({
                donatedToCharity: false,
                collectedMoney: 0,
                indexForNextRandom: 0,
                sumOfRandomNumbers: [uint(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            }));
        }
        
        // we are in reveal stage
        require(
            startOfLottery + (lotteries.length-1) * 1 weeks + 5 days < now && 
            now <= startOfLottery + (lotteries.length-1) * 1 weeks + 7 days,
            "The lottery is not in reveal stage."
        );
        
        // check if ticket belongs to message sender
        require(
            lotteries[lotteries.length-1].tickets[ticket].purchaser == msg.sender,
            "This ticket does not belong to you."
        );
        
        // if not enough money is collected
        if(lotteries[lotteries.length-1].collectedMoney < SUM_OF_PRIZES) {
            // if the money is already refunded
            if(lotteries[lotteries.length-1].tickets[ticket].refunded){
                return (false,false);
            }else{
                lotteries[lotteries.length-1].tickets[ticket].refunded = true;
                msg.sender.transfer(2 ether);
                return (false,true);
            }
        }else{ // enough money is collected
            // the commited hash is true
            if(
                lotteries[lotteries.length-1].tickets[ticket].commitment == 
                keccak256(abi.encodePacked(randomNum,msg.sender))
            ) {
                // add the provided random number to the sum
                // note: no need to check integer overflow because we only want a random number 
                lotteries[lotteries.length-1].sumOfRandomNumbers[lotteries[lotteries.length-1].indexForNextRandom] += randomNum;
                // update the index for sum for the next prize category
                lotteries[lotteries.length-1].indexForNextRandom = (lotteries[lotteries.length-1].indexForNextRandom+1)%23;
                lotteries[lotteries.length-1].tickets[ticket].revealed = true;
                // delete the commitment to prevent multiple reveals 
                lotteries[lotteries.length-1].tickets[ticket].commitment = bytes32(0);
                return (true,false);
            } else {
                // the commited hash is false
                return (false,false);
            }
        }
    }
    
    /// @notice Gives the message sender the won prize. Donates the residual money to charity if 
    /// not already done.
    /// @param lotteryWeek The week number in which the ticket is purchased. 
    /// this must be the number returned from buyTicket function
    /// @param ticketNumber The number of ticket without leading zeros that was purchased
    function withdrawPrize(uint lotteryWeek, uint ticketNumber) 
        public 
        lotteryEnded(lotteryWeek)
        lotteryExists(lotteryWeek)
        ticketBelongsToMessageSender(lotteryWeek,ticketNumber)
        randomNumberRevealed(lotteryWeek,ticketNumber)
        prizeNotCollected(lotteryWeek,ticketNumber)
    {
        if(!lotteries[lotteryWeek-1].donatedToCharity){ // if not donated
            lotteries[lotteryWeek-1].donatedToCharity = true;
            charity.transfer(lotteries[lotteryWeek-1].collectedMoney - SUM_OF_PRIZES);
        }
        
        uint prize = 0 ether;
        
        // if the least significant five digits is the same with the least significant five digits of
        // the sum of random numbers, this ticket wins the prize
        if(lotteries[lotteryWeek-1].sumOfRandomNumbers[0]%100000 == ticketNumber) {
            prize = 50000 ether; // first prize
        }else if(lotteries[lotteryWeek-1].sumOfRandomNumbers[1]%100000 == ticketNumber) {
            prize = 100000 ether;
        }else if(
            lotteries[lotteryWeek-1].sumOfRandomNumbers[2]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[3]%100000 == ticketNumber
        ) {
            prize = 400 ether;
        }else if(
            lotteries[lotteryWeek-1].sumOfRandomNumbers[4]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[5]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[6]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[7]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[8]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[9]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[10]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[11]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[12]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[13]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[14]%100000 == ticketNumber
        ) {
            prize = 200 ether;
        }else if(
            lotteries[lotteryWeek-1].sumOfRandomNumbers[15]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[16]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[17]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[18]%100000 == ticketNumber || 
            lotteries[lotteryWeek-1].sumOfRandomNumbers[19]%100000 == ticketNumber
        ) {
            prize = 100 ether;
        }else if(lotteries[lotteryWeek-1].sumOfRandomNumbers[20]%10000 == ticketNumber%10000) {
            prize = 40 ether;
        }else if(lotteries[lotteryWeek-1].sumOfRandomNumbers[21]%1000 == ticketNumber%1000) {
            prize = 10 ether;
        }else if(lotteries[lotteryWeek-1].sumOfRandomNumbers[20]%100 == ticketNumber%100) {
            prize = 4 ether;
        }
        
        lotteries[lotteryWeek-1].tickets[ticketNumber].prizeCollected = true;
        if(prize > 0 ether) {
            msg.sender.transfer(prize);
        }
    }
    
    /// @notice Returns the next available ticket number
    /// @return available ticket number
    function nextFreeTicket() public view returns(uint ticketNum) {
        for(uint i = 0; i< 100000; i++) {
            if(lotteries[lotteries.length-1].tickets[i].purchaser == address(0)) {
                return i;
            }
        }
    }
    
    /// @notice Calculates keccak256 hash from random number and account address
    /// @param randomNum a random number
    /// @param accountAddress account address
    /// @return keccak256 hash of (randomNum || accountAddress) 
    function calculateRandomNumHash(uint randomNum, address accountAddress) 
        public pure returns(bytes32 hash) 
    {
        return keccak256(abi.encodePacked(randomNum,accountAddress));
    }
}
