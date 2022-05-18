/*
 * dtp.c
 *
 *  Created on: 11 oct. 2021
 *      Author: i2CAT
 */

#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#include "du.h"
#include "common.h"
#include "rmt.h"

#define TAG_DTP "[DTP]"
/*
static dtpSv_t default_sv = {
    .xNextSeqNumberToSend = 0,        // ok
    .xMaxSeqNumberToSend = 0,         // ok
    .xSeqNumberRolloverThreshold = 0, // ok
    .xMaxSeqNumberRcvd = 0,           // ok
    .stats = {
        .drop_pdus = 0,          // ok
        .err_pdus = 0,           // ok
        .tx_pdus = 0,            // ok
        .tx_bytes = 0,           // ok
        .rx_pdus = 0,            // ok
        .rx_bytes = 0},          // ok
    .xRexmsnCtrl = false,        // ok
    .xRateBased = false,         // ok
    .xWindowBased = false,       // ok
    .xDrfRequired = true,        // ok
    .xRateFulfiled = false,      // ok
    .xMaxFlowPduSize = UINT_MAX, // ok
    .xMaxFlowSduSize = UINT_MAX, // ok
    //.MPL                  = 1000,
    //.R                    = 100,
    //.A                    = 0,
    //.tr                   = 0,
    .xRcvLeftWindowEdge = 0, // ok
    .xWindowClosed = false,  // ok
    .xDrfFlag = true,        // ok
};*/

dtpSv_t *pxDtpStateVectorInit(void);
dtpSv_t *pxDtpStateVectorInit(void)
{
        dtpSv_t *pxDtpSv;
        pxDtpSv = pvPortMalloc(sizeof(*pxDtpSv));

        pxDtpSv->xNextSeqNumberToSend = 0;        // ok
        pxDtpSv->xMaxSeqNumberToSend = 0;         // ok
        pxDtpSv->xSeqNumberRolloverThreshold = 0; // ok
        pxDtpSv->xMaxSeqNumberRcvd = 0;           // ok
        pxDtpSv->stats.drop_pdus = 0;             // ok
        pxDtpSv->stats.err_pdus = 0;              // ok
        pxDtpSv->stats.tx_pdus = 0;               // ok
        pxDtpSv->stats.tx_bytes = 0;              // ok
        pxDtpSv->stats.rx_pdus = 0;               // ok
        pxDtpSv->stats.rx_bytes = 0;
        // ok
        pxDtpSv->xRexmsnCtrl = false;        // ok
        pxDtpSv->xRateBased = false;         // ok
        pxDtpSv->xWindowBased = false;       // ok
        pxDtpSv->xDrfRequired = true;        // ok
        pxDtpSv->xRateFulfiled = false;      // ok
        pxDtpSv->xMaxFlowPduSize = UINT_MAX; // ok
        pxDtpSv->xMaxFlowSduSize = UINT_MAX; // ok
        //.MPL                  = 1000,
        //.R                    = 100,
        //.A                    = 0,
        //.tr                   = 0,
        pxDtpSv->xRcvLeftWindowEdge = 0; // ok
        pxDtpSv->xWindowClosed = false;  // ok
        pxDtpSv->xDrfFlag = true;        // ok

        return pxDtpSv;
}

BaseType_t xDtpPduSend(dtp_t *pxDtp, rmt_t *pxRmt, struct du_t *pxDu);

BaseType_t xDtpPduSend(dtp_t *pxDtp, rmt_t *pxRmt, struct du_t *pxDu)
{
        struct efcpContainer_t *pxEfcpContainer;
        cepId_t destCepId;

        ESP_LOGI(TAG_DTP, "xDtpPduSend");
        /* Remote flow case */
        if (pxDu->pxPci->xSource != pxDu->pxPci->xDestination)
        {
                /*if (dtp->dtcp->sv->rendezvous_rcvr) {
                        ESP_LOGI(TAG_DTP,"Sending to RMT in RV at RCVR");
                }*/

                if (xRmtSend(pxRmt, pxDu))
                {
                        ESP_LOGE(TAG_DTP, "Problems sending PDU to RMT");
                        return pdFALSE;
                }

                return pdTRUE;
        }

        /* Local flow case */
        destCepId = pxDu;
        pxEfcpContainer = pxDtp->pxEfcp->pxEfcpContainer;
        // pxEfcpContainer = pxDtp->pxEfcp->pxEfcpContainer;
        if (unlikely(!pxEfcpContainer || xDuDecap(pxDu) || !xDuIsOk(pxDu)))
        { /*Decap PDU */
                ESP_LOGE(TAG_DTP, "Could not retrieve the EFCP container in"
                                  "loopback operation");
                xDuDestroy(pxDu);
                return pdFALSE;
        }
        if (xEfcpContainerReceive(pxEfcpContainer, destCepId, pxDu))
        {
                ESP_LOGE(TAG_DTP, "Problems sending PDU to loopback EFCP");
                return pdFALSE;
        }

        return 0;
}

BaseType_t xDtpWrite(dtp_t *pxDtpInstance, struct du_t *pxDu)
{
        ESP_LOGI(TAG_DTP, "xDtpWrite");
        dtcp_t *pxDtcp;
        struct du_t *pxTempDu;
        struct dtp_ps *ps;
        seqNum_t xSn, xCsn;
        struct efcp_t *pxTempEfcp;
        int sbytes;
        uint_t uxSc;
        // timeout_t         mpl, r, a, rv;
        BaseType_t xStartRvTimer;

        pxTempEfcp = pxDtpInstance->pxEfcp;
        pxDtcp = pxDtpInstance->pxDtcp;

        /* Stop SenderInactivityTimer */
        /*if (rtimer_stop(&instance->timers.sender_inactivity)) {
                LOG_ERR("Failed to stop timer");
        }*/

        /*
         * FIXME: The two ways of carrying out flow control
         * could exist at once, thus reconciliation should be
         * the first and default case if both are present.
         */

        /* FIXME: from now on, this should be done for each user data
         * generated by Delimiting *
         */

        /* Step 2: Sequencing */
        /*
         * Incrementing here means the PDU cannot
         * be just thrown away from this point onwards
         */
        /* Probably needs to be revised */

        sbytes = xDuLen(pxDu);

        ESP_LOGI(TAG_DTP, "Calling DUEncap");
        ESP_LOGI(TAG_DTP, "Sbytes: %d", sbytes);
        if (!xDuEncap(pxDu, PDU_TYPE_DT))
        {
                ESP_LOGE(TAG_DTP, "Could not encap PDU");
                xDuDestroy(pxDu);
                return pdFALSE;
        }

        xCsn = ++pxDtpInstance->pxDtpStateVector->xNextSeqNumberToSend;

        pxDu->pxPci->ucVersion = 0x01;
        pxDu->pxPci->connectionId_t.xSource = pxTempEfcp->pxConnection->xSourceCepId;
        pxDu->pxPci->connectionId_t.xDestination = pxTempEfcp->pxConnection->xDestinationCepId;
        pxDu->pxPci->connectionId_t.xQosId = pxTempEfcp->pxConnection->xQosId;
        pxDu->pxPci->xDestination = pxTempEfcp->pxConnection->xDestinationAddress;
        pxDu->pxPci->xSource = pxTempEfcp->pxConnection->xSourceAddress;

        pxDu->pxPci->xFlags = 0;
        pxDu->pxPci->xType = PDU_TYPE_DT;
        pxDu->pxPci->xPduLen = pxDu->pxNetworkBuffer->xDataLength;
        pxDu->pxPci->xSequenceNumber = xCsn;

        ESP_LOGI(TAG_DTP, "------------ PCI -----------");
        ESP_LOGI(TAG_DTP, "PCI Version: 0x%04x", pxDu->pxPci->ucVersion);
        ESP_LOGI(TAG_DTP, "PCI SourceAddress: 0x%04x", pxDu->pxPci->xSource);
        ESP_LOGI(TAG_DTP, "PCI DestinationAddress: 0x%04x", pxDu->pxPci->xDestination);
        ESP_LOGI(TAG_DTP, "PCI QoS: 0x%04x", pxDu->pxPci->connectionId_t.xQosId);
        ESP_LOGI(TAG_DTP, "PCI CEP Source: 0x%04x", pxDu->pxPci->connectionId_t.xSource);
        ESP_LOGI(TAG_DTP, "PCI CEP Destination: 0x%04x", pxDu->pxPci->connectionId_t.xDestination);
        ESP_LOGI(TAG_DTP, "PCI FLAG: 0x%04x", pxDu->pxPci->xFlags);
        ESP_LOGI(TAG_DTP, "PCI Type: 0x%04x", pxDu->pxPci->xType);
        ESP_LOGI(TAG_DTP, "PCI SequenceNumber: 0x%08x", pxDu->pxPci->xSequenceNumber);
        ESP_LOGI(TAG_DTP, "PCI xPDULEN: 0x%04x", pxDu->pxPci->xPduLen);

        if (!xPciIsOk(pxDu->pxPci))
        {
                ESP_LOGE(TAG_DTP, "PCI is not ok");
                xDuDestroy(pxDu);
                return pdFALSE;
        }

        // sn = dtcp->sv->snd_lft_win;

        if (pxDtpInstance->pxDtpStateVector->xDrfFlag) //||
                                                       //((sn == (csn - 1)) && instance->sv->rexmsn_ctrl))
        {
                pduFlags_t xPciFlags;
                xPciFlags = pxDu->pxPci->xFlags;
                xPciFlags |= PDU_FLAGS_DATA_RUN;
                pxDu->pxPci->xFlags = xPciFlags;
        }
#if 0
        LOG_DBG("DTP Sending PDU %u (CPU: %d)", csn, smp_processor_id());
        mpl = instance->sv->MPL;
        r = instance->sv->R;
        a = instance->sv->A;
        spin_unlock_bh(&instance->sv_lock);

        if (dtcp) {
                rcu_read_lock();
                ps = container_of(rcu_dereference(instance->base.ps),
                                  struct dtp_ps, base);
                if (instance->sv->window_based || instance->sv->rate_based) {
			/* NOTE: Might close window */
			if (window_is_closed(instance,
						dtcp,
						csn,
						ps)) {
				if (ps->closed_window(ps, du)) {
					LOG_ERR("Problems with the closed window policy");
					goto stats_err_exit;
				}
				rcu_read_unlock();

				/* Check if rendezvous PDU needs to be sent*/
				start_rv_timer = false;
				spin_lock_bh(&instance->sv_lock);

				/* If there is rtx control and PDUs at the rtxQ
				 * don't enter the rendezvous state (DTCP will keep
				 * retransmitting the PDUs until acked or the
				 * retransmission timeout fires)
				 */
				if (instance->sv->rexmsn_ctrl &&
						rtxq_size(instance->rtxq) > 0) {
					LOG_DBG("Window is closed but there are PDUs at the RTXQ");
					spin_unlock_bh(&instance->sv_lock);
					return 0;
				}

				/* Else, check if rendezvous PDU needs to be sent */
				if (!instance->dtcp->sv->rendezvous_sndr) {
					instance->dtcp->sv->rendezvous_sndr = true;

					LOG_DBG("RV at the sender %u (CPU: %d)", csn, smp_processor_id());

					/* Start rendezvous timer, wait for Tr to fire */
					start_rv_timer = true;
					rv = jiffies_to_msecs(instance->sv->tr);
				}
				spin_unlock_bh(&instance->sv_lock);

				if (start_rv_timer) {
					LOG_DBG("Window is closed. SND LWE: %u | SND RWE: %u | TR: %u",
							instance->dtcp->sv->snd_lft_win,
							instance->dtcp->sv->snd_rt_wind_edge,
							instance->sv->tr);
					/* Send rendezvous PDU and start time */
					rtimer_start(&instance->timers.rendezvous, rv);
				}

				return 0;
			}
			if(instance->sv->rate_based) {
				spin_lock_bh(&instance->sv_lock);
				sc = dtcp->sv->pdus_sent_in_time_unit;
				if(sbytes >= 0) {
					if (sbytes + sc >= dtcp->sv->sndr_rate)
						dtcp->sv->pdus_sent_in_time_unit =
								dtcp->sv->sndr_rate;
					else
						dtcp->sv->pdus_sent_in_time_unit =
								dtcp->sv->pdus_sent_in_time_unit + sbytes;
				}
				spin_unlock_bh(&instance->sv_lock);
			}
                }
                if (instance->sv->rexmsn_ctrl) {
                        cdu = du_dup_ni(du);
                        if (!cdu) {
                                LOG_ERR("Failed to copy PDU. PDU type: %d",
                                	 pci_type(&du->pci));
                                goto pdu_stats_err_exit;
                        }

                        if (rtxq_push_ni(instance->rtxq, cdu)) {
                                LOG_ERR("Couldn't push to rtxq");
                                goto pdu_stats_err_exit;
                        }
                } else if (instance->rttq) {
                	if (rttq_push(instance->rttq, csn)) {
                		LOG_ERR("Failed to push SN to RTT queue");
                	}
                }

                if (ps->transmission_control(ps, du)) {
                        LOG_ERR("Problems with transmission control");
                        goto stats_err_exit;
                }

                rcu_read_unlock();
                spin_lock_bh(&instance->sv_lock);
                stats_inc_bytes(tx, instance->sv, sbytes);
                spin_unlock_bh(&instance->sv_lock);

                /* Start SenderInactivityTimer */
                if (rtimer_restart(&instance->timers.sender_inactivity,
                                   3 * (mpl + r + a ))) {
                        LOG_ERR("Failed to start sender_inactiviy timer");
                        goto stats_nounlock_err_exit;
                        return -1;
                }

                return 0;
        }
#endif
        if (xDtpPduSend(pxDtpInstance,
                        pxDtpInstance->pxRmt,
                        pxDu))
                return pdFALSE;
        // spin_lock_bh(&instance->sv_lock);
        // stats_inc_bytes(tx, pxDtpInstance->pxDtpStateVector, sbytes);
        // spin_unlock_bh(&instance->sv_lock);
        return pdTRUE;
}

static inline BaseType_t xDtpPduPost(dtp_t *pxDtpInstance, struct du_t *pxDu)
{
        struct efcp_t *pxEfcp;

        pxEfcp = pxDtpInstance->pxEfcp;

        if (xEfcpEnqueue(pxEfcp, pxEfcp->pxConnection->xPortId, pxDu))
        {
                ESP_LOGE(TAG_DTP, "Could not enqueue SDU to EFCP");
                return pdFALSE;
        }

        ESP_LOGI(TAG_DTP, "DTP enqueued to upper IPCP");
        return pdTRUE;
}

BaseType_t xDtpReceive(dtp_t *pxInstance, struct du_t *pxDu)
{
        // struct dtp_ps *  ps;
        dtcp_t *pxDtcp;
        struct dtcp_ps *dtcp_ps;
        seqNum_t xSeqNum;
        // timeout_t        a, r, mpl;
        seqNum_t xLWE;
        BaseType_t xInOrder;
        BaseType_t xRtxCtrl = false;
        seqNum_t xMaxSduGap;
        int sbytes;
        struct efcp_t *pxEfcp = 0;

        ESP_LOGI(TAG_DTP, "DTP receive started...");

        pxDtcp = pxInstance->pxDtcp;
        pxEfcp = pxInstance->pxEfcp;

        // spin_lock_bh(&instance->sv_lock);

        // a           = instance->sv->A;
        // r 	    = instance->sv->R;
        // mpl	    = instance->sv->MPL;
        xLWE = pxInstance->pxDtpStateVector->xRcvLeftWindowEdge;

        xInOrder = pdTRUE; // HardCode for completing the phase one
        xMaxSduGap = pxInstance->pxDtpCfg->xMaxSduGap;
        /* if (pxDtcp) {
                 dtcp_ps = dtcp_ps_get(dtcp);
                 rtx_ctrl = dtcp_ps->rtx_ctrl;
         }*/

        xSeqNum = pxDu->pxPci->xSequenceNumber;
        sbytes = xDuDataLen(pxDu);

        // LOG_DBG("local_soft_irq_pending: %d", local_softirq_pending());
        /*LOG_DBG("DTP Received PDU %u (CPU: %d)",
                seq_num, smp_processor_id());*/

        if (pxInstance->pxDtpStateVector->xDrfRequired)
        {
                /* Start ReceiverInactivityTimer */
                /* if (rtimer_restart(&instance->timers.receiver_inactivity,
                                    2 * (mpl + r + a))) {
                         ESP_LOGE(TAG_DTP,"Failed to start Receiver Inactivity timer");

                         xDuDestroy(pxDu);
                         return pdFALSE;
                 }*/

                if (pxDu->pxPci->xFlags & PDU_FLAGS_DATA_RUN)
                {
                        ESP_LOGI(TAG_DTP, "Data Run Flag");

                        pxInstance->pxDtpStateVector->xDrfRequired = pdFALSE;
                        pxInstance->pxDtpStateVector->xRcvLeftWindowEdge = xSeqNum;

                        // dtp_squeue_flush(pxInstance);
                        /*if (instance->rttq) {
                                rttq_flush(pxInstance->pxRttq);
                        }*/

                        /*if (pxDtcp) {
                                if (dtcp_sv_update(pxDtcp, &pxDu->xPci)) {
                                        ESP_LOGE(TAG_DTP, "Failed to update dtcp sv");
                                        return pdFALSE;
                                }
                        }*/

                        // dtp_send_pending_ctrl_pdus(instance);
                        // pdu_post(instance, du);
                        // stats_inc_bytes(rx, instance->sv, sbytes);

                        return pdTRUE;
                }

                ESP_LOGE(TAG_DTP, "Expecting DRF but not present, dropping PDU %d...",
                         xSeqNum);

                // stats_inc(drop, instance->sv);
                // spin_unlock_bh(&instance->sv_lock);

                xDuDestroy(pxDu);
                return pdTRUE;
        }

        /*
         * NOTE:
         *   no need to check presence of in_order or dtcp because in case
         *   they are not, LWE is not updated and always 0
         */
        if (xSeqNum <= xLWE)
        {
                /* Duplicate PDU or flow control overrun */
                ESP_LOGE(TAG_DTP, "Duplicate PDU or flow control overrun.SN: %u, LWE:%u",
                         xSeqNum, xLWE);
                // stats_inc(drop, instance->sv);

                // spin_unlock_bh(&instance->sv_lock);

                xDuDestroy(pxDu);

                if (pxDtcp)
                {
                        /* Send an ACK/Flow Control PDU with current window values */
                        /* FIXME: we have to send a Control ACK PDU, not an
                         * ack flow control one
                         */
                        /*if (dtcp_ack_flow_control_pdu_send(dtcp, LWE)) {
                                LOG_ERR("Failed to send ack/flow control pdu");
                                return -1;
                        }*/
                }
                return 0;
        }

        /* Start ReceiverInactivityTimer */
        /* if (rtimer_restart(&instance->timers.receiver_inactivity,
                            2 * (mpl + r + a ))) {
                 spin_unlock_bh(&instance->sv_lock);
                 LOG_ERR("Failed to start Receiver Inactivity timer");
                 du_destroy(du);
                 return -1;
         }*/

#if 0
        /* This is an acceptable data PDU, stop reliable ACK timer */
        if (dtcp->sv->rendezvous_rcvr) {
        	LOG_DBG("RV at receiver put to false");
        	dtcp->sv->rendezvous_rcvr = false;
        	rtimer_stop(&dtcp->rendezvous_rcv);
        }
        if (!a) {
                bool set_lft_win_edge;

                if (!in_order && !dtcp) {
                	spin_unlock_bh(&instance->sv_lock);
                        LOG_DBG("DTP Receive deliver, seq_num: %d, LWE: %d",
                                seq_num, LWE);
                        if (pdu_post(instance, du))
                                return -1;

                        return 0;
                }

                set_lft_win_edge = !(dtcp_rtx_ctrl(dtcp->cfg) &&
                                     ((seq_num -LWE) > (max_sdu_gap + 1)));
                if (set_lft_win_edge) {
                	instance->sv->rcv_left_window_edge = seq_num;
                }

                spin_unlock_bh(&instance->sv_lock);

                if (dtcp) {
                        if (dtcp_sv_update(dtcp, &du->pci)) {
                                LOG_ERR("Failed to update dtcp sv");
                                goto fail;
                        }
                        dtp_send_pending_ctrl_pdus(instance);
                        if (!set_lft_win_edge) {
                                du_destroy(du);
                                return 0;
                        }
                }

                if (pdu_post(instance, du))
                        return -1;

                spin_lock_bh(&instance->sv_lock);
		stats_inc_bytes(rx, instance->sv, sbytes);
		spin_unlock_bh(&instance->sv_lock);
                return 0;

        fail:
                du_destroy(du);
                return -1;
        }
#endif
        xLWE = pxInstance->pxDtpStateVector->xRcvLeftWindowEdge;

        ESP_LOGI(TAG_DTP, "DTP receive LWE: %u", xLWE);
        if (xSeqNum == xLWE + 1)
        {
                pxInstance->pxDtpStateVector->xRcvLeftWindowEdge = xSeqNum;

                //       ringq_push(instance->to_post, pxDu);
                xLWE = xSeqNum;
        } /*else {
                seq_queue_push_ni(instance->seqq->queue, du);
        }

        while (are_there_pdus(instance->seqq->queue, LWE)) {
                du = seq_queue_pop(instance->seqq->queue);
                if (!du)
                        break;+/
                xSeqNum = pxDu->pxPci->xSequenceNumber;
                xLWE     = xSeqNum;
                instance->sv->rcv_left_window_edge = xSeqNum;
                ringq_push(instance->to_post, du);
        }

        //spin_unlock_bh(&instance->sv_lock);

        if (dtcp) {
                if (dtcp_sv_update(dtcp, &du->pci)) {
                        LOG_ERR("Failed to update dtcp sv");
                }
        }

        dtp_send_pending_ctrl_pdus(instance);

        if (list_empty(&instance->seqq->queue->head))
                rtimer_stop(&instance->timers.a);
        else
                rtimer_start(&instance->timers.a, a/AF);

        while (!ringq_is_empty(instance->to_post)) {
                du = (struct du_t *) ringq_pop(instance->to_post);
                if (du) {
                        sbytes = du_data_len(du);
                        pdu_post(instance, du);
                        spin_lock_bh(&instance->sv_lock);
                        stats_inc_bytes(rx, instance->sv, sbytes);
                        spin_unlock_bh(&instance->sv_lock);
                }
        }*/

        ESP_LOGI(TAG_DTP, "DTP receive ended...");

        return pdTRUE;
}

BaseType_t xDtpDestroy(dtp_t *pxInstance)
{
        dtcp_t *pxDtcp = NULL;
        // struct cwq * cwq = NULL;
        // struct rtxq * rtxq = NULL;
        // struct rttq * rttq = NULL;
        BaseType_t ret = pdTRUE;

        if (!pxInstance)
                return pdFALSE;

        // spin_lock_bh(&instance->lock);

        /* Stop all the timer so they do not happen while we're freeing
           the object. */

        // rtimer_destroy(&instance->timers.a);
        /* tf_a posts workers that restart sender_inactivity timer, so the wq
         * must be flushed before destroying the timer */

        /*rtimer_stop(&instance->timers.sender_inactivity);
        rtimer_stop(&instance->timers.receiver_inactivity);
        rtimer_stop(&instance->timers.rate_window);
        rtimer_stop(&instance->timers.rtx);
        rtimer_stop(&instance->timers.rendezvous);*/

        if (pxInstance->pxDtcp)
        {
                pxDtcp = pxInstance->pxDtcp;
                pxInstance->pxDtcp = NULL; /* Useful */
        }

#if 0
       if (instance->cwq) {
                cwq = instance->cwq;
                instance->cwq = NULL; /* Useful */
        }

        if (instance->rtxq) {
        	rtxq = instance->rtxq;
        	instance->rtxq = NULL; /* Useful */
        }

        if (instance->rttq) {
        	rttq = instance->rttq;
        	instance->rttq = NULL;
        }*/

        spin_unlock_bh(&instance->lock);



        if (pxDtcp) {
        	if (xDtcpDestroy(pxDtcp)) {
        		ESP_LOGE(TAG_DTP,"Error destroying DTCP");
        		ret = pdFALSE;
        	}
        }
#endif
#if 0
        if (cwq) {
        	if (cwq_destroy(cwq)) {
        		LOG_WARN("Error destroying CWQ");
        		ret = -1;
        	}
        }

        if (rtxq) {
                if (rtxq_destroy(rtxq)) {
                        LOG_ERR("Failed to destroy rexmsn queue");
                        ret = -1;
                }
        }

        if (rttq) {
        	if (rttq_destroy(rttq)) {
        		LOG_ERR("Failed to destroy rexmsn queue");
        		ret = -1;
        	}
        }

        rtimer_destroy(&instance->timers.a);
        /* tf_a posts workers that restart sender_inactivity timer, so the wq
         * must be flushed before destroying the timer */

        rtimer_destroy(&instance->timers.sender_inactivity);
        rtimer_destroy(&instance->timers.receiver_inactivity);
        rtimer_destroy(&instance->timers.rate_window);
        rtimer_destroy(&instance->timers.rtx);
        rtimer_destroy(&instance->timers.rendezvous);

        if (instance->to_post) ringq_destroy(instance->to_post,
                               (void (*)(void *)) du_destroy);
        if (instance->to_send) ringq_destroy(instance->to_send,
                               (void (*)(void *)) du_destroy);

        if (instance->seqq) squeue_destroy(instance->seqq);
        if (instance->sv)   rkfree(instance->sv);
        if (instance->cfg) dtp_config_destroy(instance->cfg);
        rina_component_fini(&instance->base);

#endif
        // robject_del(&instance->robj);
        vPortFree(pxInstance);

        ESP_LOGI(TAG_DTP, "DTP %pK destroyed successfully", pxInstance);

        return pdTRUE;
}
/*
BaseType_t xDtpInitialSequenceNumber(dtp_t * pxInstance)
{
        dtpPs_t * pxPs;

        if (!instance) {
                LOG_ERR("Bogus instance passed");
                return -1;
        }

        rcu_read_lock();
        ps = container_of(rcu_dereference(instance->base.ps),
                          struct dtp_ps, base);
        ASSERT(ps->initial_sequence_number);
        if (ps->initial_sequence_number(ps)) {
                rcu_read_unlock();
                return -1;
        }
        rcu_read_unlock();

        return 0;
}
*/

dtp_t *pxDtpCreate(struct efcp_t *pxEfcp,
                   rmt_t *pxRmt,
                   dtpConfig_t *pxDtpCfg)
{
        dtp_t *pxDtp;
        string_t *psName;
        dtpSv_t *pxDtpSv;

        if (!pxEfcp)
        {
                ESP_LOGE(TAG_DTP, "No EFCP passed, bailing out");
                return NULL;
        }

        if (!pxDtpCfg)
        {
                ESP_LOGE(TAG_DTP, "No DTP conf passed, bailing out");
                return NULL;
        }

        if (!pxRmt)
        {
                ESP_LOGE(TAG_DTP, "No RMT passed, bailing out");
                return NULL;
        }

        pxDtp = pvPortMalloc(sizeof(*pxDtp));

        if (!pxDtp)
        {
                ESP_LOGE(TAG_DTP, "Cannot create DTP instance");
                return NULL;
        }

        pxDtp->pxEfcp = pxEfcp;

        /*if (robject_init_and_add(&dtp->robj,
                                 &dtp_rtype,
                                 parent,
                                 "dtp")) {
                dtp_destroy(dtp);
                return NULL;
        }*/
        heap_caps_check_integrity(MALLOC_CAP_DEFAULT, pdTRUE);
        // pxDtpSv = pvPortMalloc(sizeof(*pxDtpSv));
        pxDtp->pxDtpStateVector = pxDtpStateVectorInit();
        // pxDtp->pxDtpStateVector = pxDtpSv;
        heap_caps_check_integrity(MALLOC_CAP_DEFAULT, pdTRUE);
        if (!pxDtp->pxDtpStateVector)
        {
                ESP_LOGE(TAG_DTP, "Cannot create DTP state-vector");

                xDtpDestroy(pxDtp);
                return NULL;
        }
        heap_caps_check_integrity(MALLOC_CAP_DEFAULT, pdTRUE);
        //*pxDtp->pxDtpStateVector = default_sv;
        /* FIXME: fixups to the state-vector should be placed here */

        // spin_lock_init(&dtp->sv_lock);
        heap_caps_check_integrity(MALLOC_CAP_DEFAULT, pdTRUE);
        pxDtp->pxDtpCfg = pxDtpCfg;
        pxDtp->pxRmt = pxRmt;
        // dtp->rttq = NULL;
        // dtp->rtxq = NULL;
        // dtp->seqq = squeue_create(dtp);
#if 0
        if (!dtp->seqq) {
                LOG_ERR("Could not create Sequencing queue");
                dtp_destroy(dtp);
                return NULL;
        }

        rtimer_init(tf_sender_inactivity, &dtp->timers.sender_inactivity, dtp);
        rtimer_init(tf_receiver_inactivity, &dtp->timers.receiver_inactivity, dtp);
        rtimer_init(tf_a, &dtp->timers.a, dtp);
        rtimer_init(tf_rate_window, &dtp->timers.rate_window, dtp);
        rtimer_init(tf_rendezvous, &dtp->timers.rendezvous, dtp);

        dtp->to_post = ringq_create(TO_POST_LENGTH);
        if (!dtp->to_post) {
                LOG_ERR("Unable to create to_post queue; bailing out");
               dtp_destroy(dtp);
               return NULL;
        }
        dtp->to_send = ringq_create(TO_SEND_LENGTH);
        if (!dtp->to_send) {
               LOG_ERR("Unable to create to_send queue; bailing out");
               dtp_destroy(dtp);
               return NULL;
        }

        rina_component_init(&dtp->base);

        ps_name = (string_t *) policy_name(dtp_conf_ps_get(dtp_cfg));
        if (!ps_name || !strcmp(ps_name, ""))
                ps_name = RINA_PS_DEFAULT_NAME;

        if (dtp_select_policy_set(dtp, "", ps_name)) {
                ESP_LOGE(TAG_DTP,"Could not load DTP PS %s", ps_name);
                xDtpDestroy(pxDtp);
                return NULL;
        }



        if (dtp_initial_sequence_number(dtp)) {
                ESP_LOGE(TAG_DTP,"Could not create Sequencing queue");
                xDtpDestroy(pxDtp);
                return NULL;
        }

        //spin_lock_init(&dtp->lock);

#endif

        return pxDtp;
}
