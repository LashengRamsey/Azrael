/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`game` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `game`;

/*Table structure for table `account` */

DROP TABLE IF EXISTS `account`;

CREATE TABLE `account` (
  `AcntId` bigint(20) NOT NULL,
  `Account` varchar(100) DEFAULT '""',
  `Lv` int(10) DEFAULT '1',
  `Exp` int(20) DEFAULT '0',
  `Gold` int(10) DEFAULT '0',
  `VipLv` int(10) DEFAULT '0',
  `VipExp` int(10) DEFAULT '0',
  `LastLoginTime` datetime DEFAULT NULL,
  `CreateTime` datetime DEFAULT NULL,
  `Data` longtext,
  PRIMARY KEY (`AcntId`),
  KEY `Account` (`Account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `singleton` */

DROP TABLE IF EXISTS `singleton`;

CREATE TABLE `singleton` (
  `Name` varchar(30) DEFAULT NULL,
  `Data` longtext
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `test` */

DROP TABLE IF EXISTS `test`;

CREATE TABLE `test` (
  `tid` int(11) NOT NULL DEFAULT '0',
  `name` longtext,
  PRIMARY KEY (`tid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;







/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
