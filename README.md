# TcpClient
TcpClient based on unp

# TCP ACK实现
1.发送接口维护一个Counter，每次发送时更新Counter，增加发送的长度计数。
2.每次发送获取当前SNDBUFF中的数据，为0时清空Counter。
3.只要比较Counter的计数和当前获取的SNDBUFF中的数据，就知道数据有没有发出去。
注：SNDBUFF中的数据，只有收到TCP层的ACK才会删除。

#ifdef __TCPACK__
    /* add counter for TCP ACK */
    if(getBufsize() == 0 && m_counter->getCounter() > 0){
        m_counter->resetCounter(0);
    }
    m_counter->setCounter(len);
#endif
