-- phpMyAdmin SQL Dump
-- version 4.8.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: May 07, 2018 at 10:31 PM
-- Server version: 10.1.31-MariaDB
-- PHP Version: 7.2.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `cmpe321hw3`
--

DELIMITER $$
--
-- Procedures
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `completed` (`projectManagerIDinput` VARCHAR(30))  BEGIN
	IF projectManagerIDinput LIKE 'ALL' THEN
		SELECT *
		FROM   project
		WHERE  NOT EXISTS (SELECT *
		                   FROM   task
		                   WHERE  ADDDATE(StartDate, INTERVAL DayItTakes DAY) >= CURDATE()
		                          AND project.ID = task.ProjectID)
		       AND EXISTS (SELECT *
		                   FROM   task
		                   WHERE  project.ID = task.ProjectID); 
	ELSE
		SELECT *
		FROM   project
		WHERE  NOT EXISTS (SELECT *
		                   FROM   task
		                   WHERE  ADDDATE(StartDate, INTERVAL DayItTakes DAY) >= CURDATE()
		                          AND project.ID = task.ProjectID)
		       AND EXISTS (SELECT *
		                   FROM   task
		                   WHERE  project.ID = task.ProjectID)
		       AND ID IN (SELECT ProjectID as ID
		                  FROM   manageproject
		                  WHERE  ProjectManagerID = CONVERT(projectManagerIDinput, INT)); 
	END IF;
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `incomplete` (`projectManagerIDinput` VARCHAR(30))  BEGIN
	IF projectManagerIDinput LIKE 'ALL' THEN
		SELECT *
		FROM project
		WHERE ID NOT IN(
			SELECT ID
			FROM   project
			WHERE  NOT EXISTS (SELECT *
			                   FROM   task
			                   WHERE  ADDDATE(StartDate, INTERVAL DayItTakes DAY) >= CURDATE()
			                          AND project.ID = task.ProjectID)
			       AND EXISTS (SELECT *
			                   FROM   task
			                   WHERE  project.ID = task.ProjectID)); 
	ELSE
		SELECT *
		FROM project
		WHERE ID NOT IN(
			SELECT ID
			FROM   project
			WHERE  NOT EXISTS (SELECT *
			                   FROM   task
			                   WHERE  ADDDATE(StartDate, INTERVAL DayItTakes DAY) >= CURDATE()
			                          AND project.ID = task.ProjectID)
			       AND EXISTS (SELECT *
			                   FROM   task
			                   WHERE  project.ID = task.ProjectID)
			       AND ID IN (SELECT ProjectID as ID
			                  FROM   manageproject
			                  WHERE  ProjectManagerID = CONVERT(projectManagerIDinput, INT)))
		AND ID IN (SELECT ProjectID as ID
                   FROM   manageproject
                   WHERE  ProjectManagerID = CONVERT(projectManagerIDinput, INT)); 
	END IF;
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `dotask`
--

CREATE TABLE `dotask` (
  `TaskID` int(11) NOT NULL,
  `EmployeeID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `dotask`
--

INSERT INTO `dotask` (`TaskID`, `EmployeeID`) VALUES
(1, 2),
(5, 2),
(6, 2);

-- --------------------------------------------------------

--
-- Table structure for table `employee`
--

CREATE TABLE `employee` (
  `ID` int(11) NOT NULL,
  `FName` varchar(30) NOT NULL,
  `LName` varchar(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `employee`
--

INSERT INTO `employee` (`ID`, `FName`, `LName`) VALUES
(2, 'Ali', 'Veli');

--
-- Triggers `employee`
--
DELIMITER $$
CREATE TRIGGER `deleteEmployeeAssignments` AFTER DELETE ON `employee` FOR EACH ROW DELETE FROM dotask WHERE EmployeeID = OLD.ID
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `manageproject`
--

CREATE TABLE `manageproject` (
  `ProjectID` int(11) NOT NULL,
  `ProjectManagerID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `manageproject`
--

INSERT INTO `manageproject` (`ProjectID`, `ProjectManagerID`) VALUES
(1, 3),
(2, 3),
(3, 3),
(3, 4),
(4, 4),
(5, 4);

-- --------------------------------------------------------

--
-- Table structure for table `project`
--

CREATE TABLE `project` (
  `ID` int(11) NOT NULL,
  `PName` varchar(30) NOT NULL,
  `StartDate` date NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `project`
--

INSERT INTO `project` (`ID`, `PName`, `StartDate`) VALUES
(1, 'First Project', '2018-05-07'),
(2, 'Second', '2018-05-15'),
(3, 'newpro', '2018-05-21'),
(4, 'asf', '2018-05-22'),
(5, 'asfssf', '2018-05-18');

--
-- Triggers `project`
--
DELIMITER $$
CREATE TRIGGER `addProMantoProject` AFTER INSERT ON `project` FOR EACH ROW INSERT INTO manageproject(ProjectID, ProjectManagerID) 
SELECT NEW.ID, result.ID
FROM (

SELECT user.ID, 0 as ProjectCount
FROM user
WHERE user.AdminRole=0 AND NOT EXISTS (SELECT manageproject.* FROM manageproject WHERE user.ID = manageproject.ProjectManagerID)

UNION

SELECT ProjectManagerID as ID, COUNT(ProjectID) as ProjectCount
FROM manageproject
GROUP BY manageproject.ProjectManagerID

ORDER BY ProjectCount
) as result
LIMIT 1
$$
DELIMITER ;
DELIMITER $$
CREATE TRIGGER `deleteProjectAssignmentWhenProjectDeleted` AFTER DELETE ON `project` FOR EACH ROW DELETE FROM manageproject WHERE ProjectID = OLD.ID
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `task`
--

CREATE TABLE `task` (
  `ID` int(11) NOT NULL,
  `ProjectID` int(11) NOT NULL,
  `StartDate` date NOT NULL,
  `DayItTakes` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `task`
--

INSERT INTO `task` (`ID`, `ProjectID`, `StartDate`, `DayItTakes`) VALUES
(1, 1, '2018-05-01', 9),
(3, 1, '2018-05-02', 1),
(5, 4, '2018-05-04', 1),
(6, 1, '2018-04-10', 1),
(7, 1, '2018-05-03', 1);

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

CREATE TABLE `user` (
  `ID` int(11) NOT NULL,
  `Username` varchar(30) NOT NULL,
  `Password` varchar(30) NOT NULL,
  `AdminRole` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `user`
--

INSERT INTO `user` (`ID`, `Username`, `Password`, `AdminRole`) VALUES
(1, 'admin', '', 1),
(3, 'ali', '1', 0),
(4, 'abc', '123', 0);

--
-- Triggers `user`
--
DELIMITER $$
CREATE TRIGGER `deleteProManAssignmentWhenProManDeleted` AFTER DELETE ON `user` FOR EACH ROW DELETE FROM manageproject WHERE ProjectManagerID = OLD.ID
$$
DELIMITER ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `dotask`
--
ALTER TABLE `dotask`
  ADD PRIMARY KEY (`TaskID`,`EmployeeID`);

--
-- Indexes for table `employee`
--
ALTER TABLE `employee`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `manageproject`
--
ALTER TABLE `manageproject`
  ADD PRIMARY KEY (`ProjectID`,`ProjectManagerID`);

--
-- Indexes for table `project`
--
ALTER TABLE `project`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `task`
--
ALTER TABLE `task`
  ADD PRIMARY KEY (`ID`,`ProjectID`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `employee`
--
ALTER TABLE `employee`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `project`
--
ALTER TABLE `project`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `task`
--
ALTER TABLE `task`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT for table `user`
--
ALTER TABLE `user`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
